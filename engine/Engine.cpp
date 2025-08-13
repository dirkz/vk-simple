#include "Engine.h"

namespace vksimple
{

std::vector<std::string> ValidationLayers{"VK_LAYER_KHRONOS_validation"};

#if NDEBUG
constexpr bool EnableValidation = false;
#else
constexpr bool EnableValidation = true;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        const char *kind = "UNKNOWN";
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            kind = "VERBOSE";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            kind = "INFO";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            kind = "WARNING";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            kind = "ERROR";
            break;
        }
        SDL_Log("%s: %s", kind, pCallbackData->pMessage);

        return vk::True;
    }

    return vk::False;
}

Engine::Engine(IVulkanWindow &window) : m_window{window}, m_context{window.GetInstanceProcAddr()}
{
    CreateInstance();
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

    std::vector<const char *> layerNames(ValidationLayers.size());
    std::transform(ValidationLayers.begin(), ValidationLayers.end(), layerNames.begin(),
                   [](const std::string &s) { return s.c_str(); });

    vk::InstanceCreateInfo createInfo{{}, &appInfo, layerNames, extensionNames};

    m_instance = m_context.createInstance(createInfo);
}

void Engine::SetupDebugMessenger()
{
    vk::DebugUtilsMessengerCreateInfoEXT createInfo{
        {},
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding,
        DebugCallback};
}

} // namespace vksimple