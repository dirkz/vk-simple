#include "Engine.h"

namespace vksimple
{

std::vector<std::string> ValidationLayers{"VK_LAYER_KHRONOS_validation"};

#if NDEBUG
constexpr bool EnableValidation = false;
#else
constexpr bool EnableValidation = true;
#endif

Engine::Engine(IVulkanWindow &window) : m_window{window}, m_context{window.GetInstanceProcAddr()}
{
    CreateInstance();
    SetupDebugMessenger();
    PickPhysicalDevice();
    CreateLogicalDevice();
}

bool Engine::CheckValidationLayerSupport()
{
    std::set<std::string> layersPresent{};
    std::vector<vk::LayerProperties> instanceLayers = m_context.enumerateInstanceLayerProperties();
    for (vk::LayerProperties &prop : instanceLayers)
    {
        layersPresent.insert(prop.layerName);
    }

    for (std::string &layerName : ValidationLayers)
    {
        if (!layersPresent.contains(layerName))
        {
            return false;
        }
    }

    return true;
}

std::vector<std::string> Engine::GetRequiredExtensionNames()
{
    std::vector<std::string> instanceExtensions = m_window.GetInstanceExtensions();
    if (EnableValidation)
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return instanceExtensions;
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL
DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              vk::DebugUtilsMessageTypeFlagsEXT messageType,
              const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        const char *kind = "UNKNOWN";
        switch (messageSeverity)
        {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            kind = "VERBOSE";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            kind = "INFO";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            kind = "WARNING";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            kind = "ERROR";
            break;
        }
        SDL_Log("Validation (%s): %s", kind, pCallbackData->pMessage);

        return vk::True;
    }

    return vk::False;
}

vk::DebugUtilsMessengerCreateInfoEXT Engine::CreateDebugUtilsMessengerCreateInfo()
{
    const vk::DebugUtilsMessageSeverityFlagsEXT messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;

    const vk::DebugUtilsMessageTypeFlagsEXT messageType =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{
        {}, messageSeverity, messageType, DebugCallback};

    return createInfo;
}

void Engine::CreateInstance()
{
    if (EnableValidation && !CheckValidationLayerSupport())
    {
        throw std::runtime_error{"Validation requested but not supported"};
    }

    vk::ApplicationInfo appInfo{"vk-simple", VK_MAKE_VERSION(0, 0, 1), "vk-simple",
                                VK_API_VERSION_1_4};

    std::vector<std::string> instanceExtensions = GetRequiredExtensionNames();
    std::vector<const char *> extensionNames(instanceExtensions.size());
    std::transform(instanceExtensions.begin(), instanceExtensions.end(), extensionNames.begin(),
                   [](const std::string &s) { return s.c_str(); });

    std::vector<const char *> layerNames{};
    if (EnableValidation)
    {
        layerNames.resize(ValidationLayers.size());
        std::transform(ValidationLayers.begin(), ValidationLayers.end(), layerNames.begin(),
                       [](const std::string &s) { return s.c_str(); });
    }

    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo =
        CreateDebugUtilsMessengerCreateInfo();

    vk::InstanceCreateInfo instanceCreateInfo{{}, &appInfo, layerNames, extensionNames};

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> createInfo{
        instanceCreateInfo, debugMessengerCreateInfo};

    if (!EnableValidation)
    {
        createInfo.unlink<vk::DebugUtilsMessengerCreateInfoEXT>();
    }

    m_instance = m_context.createInstance(createInfo.get<vk::InstanceCreateInfo>());
}

void Engine::SetupDebugMessenger()
{
    if (!EnableValidation)
    {
        return;
    }

    vk::DebugUtilsMessengerCreateInfoEXT createInfo = CreateDebugUtilsMessengerCreateInfo();

    m_debugMessenger = vk::raii::DebugUtilsMessengerEXT{m_instance, createInfo};
}

bool Engine::IsDeviceSuitable(vk::raii::PhysicalDevice &device)
{
    QueueFamilyIndices indices{device};
    return indices.IsComplete();
}

void Engine::PickPhysicalDevice()
{
    std::vector<vk::raii::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    for (vk::raii::PhysicalDevice &device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_physicalDevice = device;
            m_queueFamilyIndices = QueueFamilyIndices{device};
            break;
        }
    }

    if (m_physicalDevice == nullptr)
    {
        throw std::runtime_error{"no suitable physical device found"};
    }
}

void Engine::CreateLogicalDevice()
{
    const std::array priorities{1.0f};
    vk::DeviceQueueCreateInfo queueCreateInfo{{}, m_queueFamilyIndices.GraphicsQueue(), priorities};

    vk::PhysicalDeviceFeatures deviceFeatures{};

    std::vector<const char *> layerNames{};
    if (EnableValidation)
    {
        layerNames.resize(ValidationLayers.size());
        std::transform(ValidationLayers.begin(), ValidationLayers.end(), layerNames.begin(),
                       [](const std::string &s) { return s.c_str(); });
    }

    vk::DeviceCreateInfo createInfo{{}, queueCreateInfo, layerNames};

    m_device = m_physicalDevice.createDevice(createInfo);

    m_graphicsQueue = m_device.getQueue(m_queueFamilyIndices.GraphicsQueue(), 0);
}

} // namespace vksimple