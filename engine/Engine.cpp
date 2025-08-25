#include "Engine.h"

#include "Buffer.h"
#include "ShaderModuleLoader.h"
#include "SwapchainSupportDetails.h"
#include "Vertex.h"

namespace vksimple
{

std::vector<std::string> ValidationLayers{"VK_LAYER_KHRONOS_validation"};

#if NDEBUG
constexpr bool EnableValidation = false;
#else
constexpr bool EnableValidation = true;
#endif

std::vector<std::string> DeviceExtensions{vk::KHRSwapchainExtensionName,
                                          vk::KHRSpirv14ExtensionName,
                                          vk::KHRShaderFloatControlsExtensionName};

const std::vector<Vertex> Vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                   {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                   {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

Engine::Engine(IVulkanWindow &window) : m_window{window}, m_context{window.GetInstanceProcAddr()}
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapchain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFrameBuffers();
    CreateCommandPool();
    CreateVertexBuffer();
    CreateFrameData();
}

void Engine::DrawFrame()
{
    FrameData &frameData = CurrentFrameData();
    vk::raii::Fence &inflightFence = frameData.InflightFence();
    vk::raii::Semaphore &imageAvailableSemaphore = frameData.ImageAvailableSemaphore();
    vk::raii::CommandBuffer &commandBuffer = frameData.CommandBuffer();

    vk::Result resultOfWaiting =
        m_device.waitForFences(*inflightFence, vk::True, std::numeric_limits<uint32_t>::max());

    auto [result, imageIndex] = m_swapchain.AcquireNextImage(imageAvailableSemaphore);

    if (result == vk::Result::eErrorOutOfDateKHR || m_windowResized)
    {
        m_windowResized = false;
        RecreateSwapchain();

        // This semaphore is now burnt, renew it.
        frameData.RecreateSemaphore(m_device, imageAvailableSemaphore);

        return;
    }

    m_device.resetFences(*inflightFence);

    commandBuffer.reset();
    RecordCommandBuffer(commandBuffer, imageIndex);

    vk::raii::Semaphore &renderFinishedSemaphore =
        m_swapchain.RenderFinishedSemaphoreAt(imageIndex);

    constexpr vk::PipelineStageFlags waitDstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo{*imageAvailableSemaphore, waitDstStageMask, *commandBuffer,
                              *renderFinishedSemaphore};
    m_graphicsQueue.submit(submitInfo, inflightFence);

    const vk::raii::SwapchainKHR &swapchain = m_swapchain.SwapchainKHR();
    vk::PresentInfoKHR presentInfo{*renderFinishedSemaphore, *swapchain, imageIndex};
    vk::Result resultOfPresenting = m_presentQueue.presentKHR(presentInfo);

    m_currentFrame = (m_currentFrame + 1) % MaxFramesInFlight;
}

void Engine::WaitIdle()
{
    m_device.waitIdle();
}

void Engine::WindowResized()
{
    m_windowResized = true;
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

    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

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

        return vk::False;
    }

    return vk::False;
}

vk::DebugUtilsMessengerCreateInfoEXT Engine::CreateDebugUtilsMessengerCreateInfo()
{
    constexpr vk::DebugUtilsMessageSeverityFlagsEXT messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;

    constexpr vk::DebugUtilsMessageTypeFlagsEXT messageType =
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
                                VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_4};

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

void Engine::CreateSurface()
{
    m_surface = m_window.CreateSurface(m_instance);
}

bool Engine::CheckDeviceExtensionSupport(vk::raii::PhysicalDevice &device)
{
    const std::vector<vk::ExtensionProperties> properties =
        device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions{DeviceExtensions.begin(), DeviceExtensions.end()};

    for (const vk::ExtensionProperties &props : properties)
    {
        requiredExtensions.erase(props.extensionName);
        if (requiredExtensions.empty())
        {
            break;
        }
    }

    return requiredExtensions.empty();
}

bool Engine::IsDeviceSuitable(vk::raii::PhysicalDevice &device)
{
    QueueFamilyIndices indices{device, m_surface};

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapchainAdequate = false;
    if (extensionsSupported)
    {
        SwapchainSupportDetails swapchainDetails{device, m_surface};
        swapchainAdequate = swapchainDetails.IsAdequate();
    }

    return indices.IsComplete() && extensionsSupported && swapchainAdequate;
}

void Engine::PickPhysicalDevice()
{
    std::vector<vk::raii::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    for (vk::raii::PhysicalDevice &device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_physicalDevice = device;
            m_queueFamilyIndices = QueueFamilyIndices{device, m_surface};
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
    std::set<uint32_t> queues{m_queueFamilyIndices.GraphicsQueue(),
                              m_queueFamilyIndices.PresentQueue()};

    constexpr std::array priorities{1.0f};
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
    for (uint32_t queue : queues)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo{{}, queue, priorities};
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    std::vector<const char *> layerNames{};
    if (EnableValidation)
    {
        layerNames.resize(ValidationLayers.size());
        std::transform(ValidationLayers.begin(), ValidationLayers.end(), layerNames.begin(),
                       [](const std::string &s) { return s.c_str(); });
    }

    std::vector<const char *> extensionNames(DeviceExtensions.size());
    std::transform(DeviceExtensions.begin(), DeviceExtensions.end(), extensionNames.begin(),
                   [](const std::string &s) { return s.c_str(); });

    vk::DeviceCreateInfo createInfo{{}, queueCreateInfos, layerNames, extensionNames};

    m_device = m_physicalDevice.createDevice(createInfo);

    m_graphicsQueue = m_device.getQueue(m_queueFamilyIndices.GraphicsQueue(), 0);
    m_presentQueue = m_device.getQueue(m_queueFamilyIndices.PresentQueue(), 0);
}

void Engine::CreateSwapchain()
{
    m_swapchain = Swapchain{m_physicalDevice,
                            m_device,
                            m_surface,
                            m_window,
                            m_queueFamilyIndices.GraphicsQueue(),
                            m_queueFamilyIndices.PresentQueue()};
}

void Engine::CreateImageViews()
{
    m_swapchain.CreateImageViews(m_device);
}

void Engine::CreateRenderPass()
{
    vk::AttachmentDescription colorAttachment{{},
                                              m_swapchain.Format(),
                                              vk::SampleCountFlagBits::e1,
                                              vk::AttachmentLoadOp::eClear,     // loadOp
                                              vk::AttachmentStoreOp::eStore,    // storeOp
                                              vk::AttachmentLoadOp::eDontCare,  // stencilLoadOp
                                              vk::AttachmentStoreOp::eDontCare, // stencilStoreOp
                                              vk::ImageLayout::eUndefined,
                                              vk::ImageLayout::ePresentSrcKHR};

    vk::AttachmentReference colorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};

    vk::SubpassDescription subpass{{},
                                   vk::PipelineBindPoint::eGraphics,
                                   {}, // inputAttachments
                                   colorAttachmentRef};

    vk::SubpassDependency subpassDependency{VK_SUBPASS_EXTERNAL,
                                            0,
                                            vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                            vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                            vk::AccessFlagBits::eNone,
                                            vk::AccessFlagBits::eColorAttachmentWrite};

    vk::RenderPassCreateInfo renderPassCreateInfo{{}, colorAttachment, subpass, subpassDependency};

    m_renderPass = m_device.createRenderPass(renderPassCreateInfo);
}

void Engine::CreateGraphicsPipeline()
{
    ShaderModuleLoader shaderModuleLoader{"shader.slang.spv"};

    vk::raii::ShaderModule shaderModule = shaderModuleLoader.CreateShaderModule(m_device);

    vk::PipelineShaderStageCreateInfo vertextShaderStageCreateInfo{
        {}, vk::ShaderStageFlagBits::eVertex, shaderModule, "VertexMain"};

    vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{
        {}, vk::ShaderStageFlagBits::eFragment, shaderModule, "FragmentMain"};

    std::array shaderStageCreateInfos{vertextShaderStageCreateInfo, fragmentShaderStageCreateInfo};

    vk::VertexInputBindingDescription bindingDescrption = Vertex::BindingDescription();
    std::array attributeDescriptions = Vertex::AttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
        {}, bindingDescrption, attributeDescriptions};

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{
        {}, vk::PrimitiveTopology::eTriangleList, vk::False};

    vk::PipelineTessellationStateCreateInfo tesselationStateCreateInfo{};

    vk::Viewport viewport = m_swapchain.Viewport();

    vk::Rect2D scissorRect = m_swapchain.ScissorRect();

    std::array dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{{}, dynamicStates};

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{{}, viewport, scissorRect};

    constexpr vk::Bool32 depthClampEnable = vk::False;
    constexpr vk::Bool32 rasterizerDiscardEnable = vk::False;
    constexpr vk::Bool32 depthBiasEnable = vk::False;
    constexpr float depthBiasConstantFactor = 0.f;
    constexpr float depthBiasClamp = 0.f;
    constexpr float depthBiasSlopeFactor = 0.f;
    constexpr float lineWidth = 1.f;
    vk::PipelineRasterizationStateCreateInfo rasterizerStateCreateInfo{{},
                                                                       depthClampEnable,
                                                                       rasterizerDiscardEnable,
                                                                       vk::PolygonMode::eFill,
                                                                       vk::CullModeFlagBits::eBack,
                                                                       vk::FrontFace::eClockwise,
                                                                       depthBiasEnable,
                                                                       depthBiasConstantFactor,
                                                                       depthBiasClamp,
                                                                       depthBiasSlopeFactor,
                                                                       lineWidth};

    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};

    // Even without any blending, have to make sure all colors are written out.
    // https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions#page_Color-blending
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.colorWriteMask =
        vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB;

    constexpr vk::Bool32 logicOpEnable = vk::False;
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
        {},
        logicOpEnable,
        vk::LogicOp::eClear, // should not matter, since disabled
        colorBlendAttachmentState};

    // No uniform buffers, no push constants yet.
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};

    m_pipelineLayout = m_device.createPipelineLayout(pipelineLayoutCreateInfo);

    constexpr uint32_t subpass = 0;
    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{{},
                                                              shaderStageCreateInfos,
                                                              &vertexInputStateCreateInfo,
                                                              &inputAssemblyStateCreateInfo,
                                                              &tesselationStateCreateInfo,
                                                              &viewportStateCreateInfo,
                                                              &rasterizerStateCreateInfo,
                                                              &multisampleStateCreateInfo,
                                                              &depthStencilCreateInfo,
                                                              &colorBlendStateCreateInfo,
                                                              &dynamicStateCreateInfo,
                                                              m_pipelineLayout,
                                                              m_renderPass,
                                                              subpass};

    m_pipeline = m_device.createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo);
}

void Engine::CreateFrameBuffers()
{
    m_swapchain.CreateFrameBuffers(m_device, m_renderPass);
}

void Engine::CreateCommandPool()
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo{
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_queueFamilyIndices.GraphicsQueue()};

    m_commandPool = m_device.createCommandPool(commandPoolCreateInfo);
}

void Engine::CreateVertexBuffer()
{
    vk::BufferCreateInfo bufferCreateInfo{{},
                                          sizeof(Vertex) * Vertices.size(),
                                          vk::BufferUsageFlagBits::eVertexBuffer,
                                          vk::SharingMode::eExclusive};

    m_vertexBuffer = m_device.createBuffer(bufferCreateInfo);

    vk::MemoryRequirements memoryRequirements = m_vertexBuffer.getMemoryRequirements();

    const uint32_t memoryTypeIndex = Buffer::FindMemoryType(
        m_physicalDevice, memoryRequirements.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    vk::MemoryAllocateInfo allocateInfo{memoryRequirements.size, memoryTypeIndex};

    m_vertexBufferMemory = m_device.allocateMemory(allocateInfo);

    vk::BindBufferMemoryInfo bindBufferMemoryInfo{m_vertexBuffer, m_vertexBufferMemory, 0};
    m_device.bindBufferMemory2(bindBufferMemoryInfo);

    vk::MemoryMapInfo memoryMapInfo{{}, m_vertexBufferMemory, 0, bufferCreateInfo.size};
    void *vertexData = m_device.mapMemory2(memoryMapInfo);
    memcpy(vertexData, Vertices.data(), bufferCreateInfo.size);

    vk::MemoryUnmapInfo memoryUnmapInfo{{}, m_vertexBufferMemory};
    m_device.unmapMemory2(memoryUnmapInfo);
}

void Engine::CreateFrameData()
{
    for (auto i = 0; i < m_frameData.size(); ++i)
    {
        m_frameData[i] = FrameData(m_device, m_commandPool);
    }
}

void Engine::RecreateSwapchain()
{
    WaitIdle();

    CreateSwapchain();
    CreateImageViews();
    CreateFrameBuffers();
}

void Engine::RecordCommandBuffer(vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBuffer.begin(commandBufferBeginInfo);

    const vk::Rect2D renderArea{{0, 0}, m_swapchain.Extent()};
    constexpr vk::ClearValue clearValue{{0.f, 0.f, 0.f, 1.f}};
    vk::RenderPassBeginInfo renderPassBeginInfo{m_renderPass, m_swapchain.FrameBufferAt(imageIndex),
                                                renderArea, clearValue};
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    commandBuffer.bindVertexBuffers(0, *m_vertexBuffer, static_cast<vk::DeviceSize>(0));
    commandBuffer.setViewport(0, m_swapchain.Viewport());
    commandBuffer.setScissor(0, m_swapchain.ScissorRect());

    commandBuffer.draw(3, 1, 0, 0);

    commandBuffer.endRenderPass();
    commandBuffer.end();
}

} // namespace vksimple