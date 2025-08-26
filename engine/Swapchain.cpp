#include "Swapchain.h"

#include "SwapchainSupportDetails.h"

namespace vkdeck
{

Swapchain::Swapchain(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device,
                     vk::raii::SurfaceKHR &surface, IVulkanWindow &window, uint32_t graphicsQueue,
                     uint32_t presentQueue)
{
    SwapchainSupportDetails details{physicalDevice, surface};

    uint32_t imageCount = 3;

    if (imageCount < details.MinImageCount())
    {
        imageCount = details.MinImageCount();
    }

    if (details.MaxImageCount() > 0 && imageCount > details.MaxImageCount())
    {
        imageCount = details.MaxImageCount();
    }

    const vk::SharingMode imageSharingMode =
        graphicsQueue == presentQueue ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;
    std::vector<uint32_t> queueFamilyIndices{};
    if (imageSharingMode == vk::SharingMode::eConcurrent)
    {
        queueFamilyIndices.push_back(graphicsQueue);
        queueFamilyIndices.push_back(presentQueue);
    }

    const vk::SurfaceFormatKHR surfaceFormat = details.ChooseSurfaceFormat();
    const vk::Format imageFormat = surfaceFormat.format;
    const vk::ColorSpaceKHR imageColorSpace = surfaceFormat.colorSpace;
    const vk::Extent2D imageExtent = details.ChooseExtent(window);
    const uint32_t imageArrayLayers = 1;
    const vk::SurfaceTransformFlagBitsKHR preTransform = details.CurrentTransform();
    const vk::PresentModeKHR presentMode = details.ChoosePresentMode();
    const vk::Bool32 clipped = vk::True;
    vk::SwapchainCreateInfoKHR createInfo{{},
                                          surface,
                                          imageCount,
                                          imageFormat,
                                          imageColorSpace,
                                          imageExtent,
                                          imageArrayLayers,
                                          vk::ImageUsageFlagBits::eColorAttachment,
                                          imageSharingMode,
                                          queueFamilyIndices,
                                          preTransform,
                                          vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                          presentMode,
                                          clipped};

    m_swapchain = device.createSwapchainKHR(createInfo);

    m_images = m_swapchain.getImages();

    m_imageFormat = surfaceFormat.format;
    m_extent = imageExtent;

    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    for (uint32_t i = 0; i < imageCount; ++i)
    {
        vk::raii::Semaphore renderFinishedSemaphore = device.createSemaphore(semaphoreCreateInfo);
        m_renderFinishedSemaphores.push_back(std::move(renderFinishedSemaphore));
    }
}

void Swapchain::CreateImageViews(vk::raii::Device &device)
{
    vk::ComponentMapping componentMapping{
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity};

    vk::ImageSubresourceRange subresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    m_imageViews.clear();

    for (auto i = 0; i < m_images.size(); ++i)
    {
        vk::ImageViewCreateInfo createInfo{{},
                                           m_images[i],
                                           vk::ImageViewType::e2D,
                                           m_imageFormat,
                                           componentMapping,
                                           subresourceRange};

        vk::raii::ImageView imageView = device.createImageView(createInfo);
        m_imageViews.push_back(std::move(imageView));
    }
}

void Swapchain::CreateFrameBuffers(vk::raii::Device &device, vk::raii::RenderPass &renderPass)
{
    m_frameBuffers.clear();

    const uint32_t layers = 1;
    for (auto i = 0; i < m_imageViews.size(); ++i)
    {
        vk::FramebufferCreateInfo frameBufferCreateInfo{{},      renderPass, *m_imageViews[i],
                                                        Width(), Height(),   layers};
        vk::raii::Framebuffer frameBuffer = device.createFramebuffer(frameBufferCreateInfo);
        m_frameBuffers.push_back(std::move(frameBuffer));
    }
}

std::pair<vk::Result, uint32_t> Swapchain::AcquireNextImage(
    vk::raii::Semaphore &imageAvailableSemaphore)
{
    auto [result, imageIndex] = m_swapchain.acquireNextImage(std::numeric_limits<uint32_t>::max(),
                                                             *imageAvailableSemaphore);

    switch (result)
    {
    case vk::Result::eSuccess:
    case vk::Result::eSuboptimalKHR:
    case vk::Result::eErrorOutOfDateKHR:
        // it's either ok, or has to be handled by the caller
        break;
    default:
        char *string;
        SDL_asprintf(&string, "acquireNextImage2KHR failed with result: %s",
                     string_VkResult(static_cast<VkResult>(result)));
        SDL_Log("%s", string);
        std::string msg{string};
        SDL_free(string);
        throw std::runtime_error{msg};
    }

    return std::make_pair(result, imageIndex);
}

} // namespace vkdeck