#include "Engine.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Buffer.h"
#include "DebugUtils.h"
#include "ShaderModuleLoader.h"
#include "SwapchainSupportDetails.h"
#include "UniformObject.h"
#include "Vertex.h"

namespace vkdeck
{

constexpr vk::Format TextureFormat = vk::Format::eR8G8B8A8Srgb;

std::vector<std::string> ValidationLayers{"VK_LAYER_KHRONOS_validation"};

#if NDEBUG
constexpr bool EnableValidation = false;
#else
constexpr bool EnableValidation = true;
#endif

std::vector<std::string> PhysicalDeviceExtensions{vk::KHRSwapchainExtensionName,
                                                  vk::KHRSpirv14ExtensionName,
                                                  vk::KHRShaderFloatControlsExtensionName};

const std::vector<Vertex> Vertices{{{-0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.f, 0.f}},
                                   {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.f, 0.f}},
                                   {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.f, 1.f}},
                                   {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f}},
                                   {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.f, 0.f}},
                                   {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.f, 0.f}},
                                   {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.f, 1.f}},
                                   {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f}}};

const std::vector<uint16_t> Indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7};

Engine::Engine(IVulkanWindow &window) : m_window{window}, m_context{window.GetInstanceProcAddr()}
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateVma();
    CreateSwapchain();
    CreateImageViews();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline();
    CreateFrameBuffers();
    CreateCommandPool();

    StagingCommandPool stagingCommandPool =
        StagingCommandPool{m_device, m_graphicsQueue, m_vma, m_queueFamilyIndices.GraphicsQueue()};

    // These creation methods set the corresponding buffer/texture member as a side
    // effect and return the temporary staging buffer.
    // This temporary buffer must be held unto until the upload has been completed.
    VmaBuffer tmpDepthResourcesStagingBuffer = CreateDepthResources(stagingCommandPool);
    VmaBuffer tmpTextureStagingBuffer = CreateTextureImage(stagingCommandPool);
    VmaBuffer tmpVertexStagingBuffer = CreateVertexBuffer(stagingCommandPool);
    VmaBuffer tmpIndexStagingBuffer = CreateIndexBuffer(stagingCommandPool);

    stagingCommandPool.WaitForFences(m_device);

    // The temporary staging buffers are now not needed anymore,
    // they will be discarded at the end of this method automatically.

    CreateTextureImageView();
    CreateTextureSampler();

    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateFrameData();
}

void Engine::DrawFrame()
{
    FrameData &frameData = CurrentFrameData();

    UpdateUniformBuffer(frameData);

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
    RecordCommandBuffer(commandBuffer, frameData, imageIndex);

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

vk::DebugUtilsMessengerCreateInfoEXT Engine::CreateDebugUtilsMessengerCreateInfo()
{
    return DebugUtils::CreateDebugUtilsMessengerCreateInfo();
}

void Engine::CreateInstance()
{
    if (EnableValidation && !CheckValidationLayerSupport())
    {
        throw std::runtime_error{"Validation requested but not supported"};
    }

    vk::ApplicationInfo appInfo{"vkdeck", VK_MAKE_VERSION(0, 0, 1), "vkdeck",
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

    std::set<std::string> requiredExtensions{PhysicalDeviceExtensions.begin(),
                                             PhysicalDeviceExtensions.end()};

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
    vk::PhysicalDeviceFeatures features = device.getFeatures();
    if (features.samplerAnisotropy != vk::True)
    {
        return false;
    }

    auto features2 = device.template getFeatures2<vk::PhysicalDeviceFeatures2,
                                                  vk::PhysicalDeviceBufferDeviceAddressFeatures>();
    vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures =
        features2.get<vk::PhysicalDeviceBufferDeviceAddressFeatures>();

    if (!bufferDeviceAddressFeatures.bufferDeviceAddress)
    {
        return false;
    }

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
    deviceFeatures.samplerAnisotropy = vk::True;

    std::vector<const char *> layerNames{};
    if (EnableValidation)
    {
        layerNames.resize(ValidationLayers.size());
        std::transform(ValidationLayers.begin(), ValidationLayers.end(), layerNames.begin(),
                       [](const std::string &s) { return s.c_str(); });
    }

    // Those have already been checked to be supported.
    std::vector<std::string> deviceExtensions = PhysicalDeviceExtensions;

    // Get desired VMA extensions that are supported by the physical device
    // and add them to our list.
    std::set<std::string> vmaExtensions = Vma::DesiredPhysicalDeviceExtensions(m_physicalDevice);
    for (const std::string &vmaExtension : vmaExtensions)
    {
        deviceExtensions.push_back(vmaExtension);
    }

    std::vector<const char *> extensionNames(deviceExtensions.size());
    std::transform(deviceExtensions.begin(), deviceExtensions.end(), extensionNames.begin(),
                   [](const std::string &s) { return s.c_str(); });

    vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
    bufferDeviceAddressFeatures.bufferDeviceAddress = vk::True;

    vk::DeviceCreateInfo createInfo{
        {}, queueCreateInfos, layerNames, extensionNames, &deviceFeatures};

    vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceBufferDeviceAddressFeatures>
        createChain{createInfo, bufferDeviceAddressFeatures};

    m_device = m_physicalDevice.createDevice(createChain.get<vk::DeviceCreateInfo>());

    m_graphicsQueue = m_device.getQueue(m_queueFamilyIndices.GraphicsQueue(), 0);
    m_presentQueue = m_device.getQueue(m_queueFamilyIndices.PresentQueue(), 0);
}

void Engine::CreateVma()
{
    m_vma = Vma{m_window.GetInstanceProcAddr(), m_instance, m_physicalDevice, m_device};
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

void Engine::CreateDescriptorSetLayout()
{
    constexpr uint32_t binding = 0;
    vk::DescriptorSetLayoutBinding uniformLayoutBinding{
        binding, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, {}};
    uniformLayoutBinding.descriptorCount = 1;

    vk::DescriptorSetLayoutBinding samplerLayoutBinding{binding + 1,
                                                        vk::DescriptorType::eCombinedImageSampler,
                                                        vk::ShaderStageFlagBits::eFragment,
                                                        {}};
    samplerLayoutBinding.descriptorCount = 1;

    std::array bindings{uniformLayoutBinding, samplerLayoutBinding};
    vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{{}, bindings};

    m_descriptorSetLayout = m_device.createDescriptorSetLayout(layoutCreateInfo);
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

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{{}, *m_descriptorSetLayout};

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

vk::Format Engine::FindSupportedFormat(const std::vector<vk::Format> &candidates,
                                       vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (const vk::Format &format : candidates)
    {
        vk::FormatProperties properties = m_physicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear &&
            (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal &&
                 (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }
    throw std::runtime_error{"failed to find supported format"};
}

vk::Format Engine::FindDepthFormat()
{
    return FindSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

VmaBuffer Engine::CreateDepthResources(StagingCommandPool &stagingCommandPool)
{
    // TODO: Fake it in order to make this compile (and run)
    return m_vma.CreateBuffer(10, vk::BufferUsageFlagBits::eTransferSrc,
                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                  VMA_ALLOCATION_CREATE_MAPPED_BIT);
}

VmaBuffer Engine::CreateTextureImage(StagingCommandPool &stagingCommandPool)
{
    std::filesystem::path basePath{sdl::GetBasePath()};
    std::filesystem::path texturePath = basePath / "textures" / "texture.jpg";
    std::string texturePathString = texturePath.string();

    int texWidth, texHeight, texChannels;
    stbi_uc *pixels =
        stbi_load(texturePathString.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels)
    {
        throw std::runtime_error{"cannot load texture"};
    }

    if (texChannels != 3)
    {
        throw std::runtime_error{"unsupported texture format, invalid number of channels"};
    }

    vk::DeviceSize imageSize{static_cast<vk::DeviceSize>(texWidth) * texHeight * 4};

    VmaBuffer stagingBuffer = m_vma.CreateBuffer(
        imageSize, vk::BufferUsageFlagBits::eTransferSrc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

    stagingBuffer.CopyMemoryToAllocation(pixels);

    stbi_image_free(pixels);

    m_textureImage =
        m_vma.CreateImage(texWidth, texHeight, TextureFormat, vk::ImageTiling::eOptimal,
                          vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);

    stagingCommandPool.TransitionImageLayout(m_textureImage.Image(), TextureFormat,
                                             vk::ImageLayout::eUndefined,
                                             vk::ImageLayout::eTransferDstOptimal);

    stagingCommandPool.CopyBufferToImage(stagingBuffer.Buffer(), m_textureImage.Image(), texWidth,
                                         texHeight);

    stagingCommandPool.TransitionImageLayout(m_textureImage.Image(), TextureFormat,
                                             vk::ImageLayout::eTransferDstOptimal,
                                             vk::ImageLayout::eShaderReadOnlyOptimal);

    return stagingBuffer;
}

VmaBuffer Engine::CreateVertexBuffer(StagingCommandPool &stagingCommandPool)
{
    vk::DeviceSize bufferSize = sizeof(Vertex) * Vertices.size();

    auto [vertexBuffer, stagingBuffer] = stagingCommandPool.CreateDeviceBufferFromMemory(
        Vertices.data(), bufferSize, vk::BufferUsageFlagBits::eVertexBuffer);

    m_vertexBuffer = std::move(vertexBuffer);

    return std::move(stagingBuffer);
}

VmaBuffer Engine::CreateIndexBuffer(StagingCommandPool &stagingCommandPool)
{
    vk::DeviceSize bufferSize = sizeof(uint16_t) * Indices.size();

    auto [indexBuffer, stagingBuffer] = stagingCommandPool.CreateDeviceBufferFromMemory(
        Indices.data(), bufferSize, vk::BufferUsageFlagBits::eIndexBuffer);

    m_indexBuffer = std::move(indexBuffer);

    return std::move(stagingBuffer);
}

void Engine::CreateTextureImageView()
{
    m_textureImageView =
        Swapchain::CreateImageView(m_device, m_textureImage.Image(), TextureFormat);
}

void Engine::CreateTextureSampler()
{
    vk::PhysicalDeviceProperties deviceProperties = m_physicalDevice.getProperties();

    constexpr float mipLodBias = 0.f;
    constexpr float minLod = 0.f;
    constexpr float maxLod = 0.f;
    constexpr vk::Bool32 anisotropyEnable = vk::True;
    constexpr vk::Bool32 compareEnable = vk::False;
    constexpr vk::Bool32 unnormalizedCoordinates = vk::False;
    vk::SamplerCreateInfo createInfo{{},
                                     vk::Filter::eLinear,
                                     vk::Filter::eLinear,
                                     vk::SamplerMipmapMode::eLinear,
                                     vk::SamplerAddressMode::eRepeat,
                                     vk::SamplerAddressMode::eRepeat,
                                     vk::SamplerAddressMode::eRepeat,
                                     mipLodBias,
                                     anisotropyEnable,
                                     deviceProperties.limits.maxSamplerAnisotropy,
                                     compareEnable,
                                     vk::CompareOp::eAlways,
                                     minLod,
                                     maxLod,
                                     vk::BorderColor::eIntOpaqueBlack,
                                     unnormalizedCoordinates};

    m_textureSampler = m_device.createSampler(createInfo);
}

void Engine::CreateDescriptorPool()
{
    vk::DescriptorPoolSize uniformDescriptorPoolSize{vk::DescriptorType::eUniformBuffer,
                                                     MaxFramesInFlight};
    vk::DescriptorPoolSize samplerDescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler,
                                                     MaxFramesInFlight};
    std::array poolSizes{uniformDescriptorPoolSize, samplerDescriptorPoolSize};

    vk::DescriptorPoolCreateInfo poolCreateInfo{
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, MaxFramesInFlight, poolSizes};

    m_descriptorPool = m_device.createDescriptorPool(poolCreateInfo);
}

void Engine::CreateDescriptorSets()
{
    std::vector<vk::DescriptorSetLayout> setLayouts{MaxFramesInFlight, m_descriptorSetLayout};
    vk::DescriptorSetAllocateInfo allocateInfo{m_descriptorPool, setLayouts};
    m_descriptorSets = m_device.allocateDescriptorSets(allocateInfo);
}

void Engine::CreateFrameData()
{
    for (auto i = 0; i < MaxFramesInFlight; ++i)
    {
        m_frameData.emplace_back(m_device, m_commandPool, m_vma, m_descriptorSets[i],
                                 m_textureSampler, m_textureImageView);
    }

    m_descriptorSets.clear();
}

void Engine::RecreateSwapchain()
{
    WaitIdle();

    CreateSwapchain();
    CreateImageViews();
    CreateFrameBuffers();
}

void Engine::UpdateUniformBuffer(FrameData &frameData)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    float ratio = m_swapchain.Ratio();

    glm::mat4 model =
        glm::rotate(glm::mat4{1.f}, time * glm::radians(5.f), glm::vec3{0.f, 0.f, 1.f});
    glm::mat4 view =
        glm::lookAt(glm::vec3{0.f, -2.f, 2.f}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f});
    glm::mat4 projection = glm::perspective(glm::radians(45.f), ratio, 0.1f, 10.f);
    UniformObject ubo{model, view, projection};

    frameData.UniformBuffer().CopyMemoryToAllocation(&ubo);
}

void Engine::RecordCommandBuffer(vk::raii::CommandBuffer &commandBuffer, FrameData &frameData,
                                 uint32_t imageIndex)
{
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBuffer.begin(commandBufferBeginInfo);

    const vk::Rect2D renderArea{{0, 0}, m_swapchain.Extent()};
    constexpr vk::ClearValue clearValue{{0.f, 0.f, 0.f, 1.f}};
    vk::RenderPassBeginInfo renderPassBeginInfo{m_renderPass, m_swapchain.FrameBufferAt(imageIndex),
                                                renderArea, clearValue};
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    commandBuffer.bindVertexBuffers(0, m_vertexBuffer.Buffer(), static_cast<vk::DeviceSize>(0));
    commandBuffer.bindIndexBuffer(m_indexBuffer.Buffer(), 0, vk::IndexType::eUint16);
    commandBuffer.setViewport(0, m_swapchain.Viewport());
    commandBuffer.setScissor(0, m_swapchain.ScissorRect());
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0,
                                     {frameData.DescriptorSet()}, {});

    commandBuffer.drawIndexed(static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

    commandBuffer.endRenderPass();
    commandBuffer.end();
}

FrameData &Engine::CurrentFrameData()
{
    return m_frameData[m_currentFrame];
}

} // namespace vkdeck