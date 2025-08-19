#include "Swapchain.h"

#include "SwapchainSupportDetails.h"

namespace vksimple
{

vksimple::Swapchain::Swapchain(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device,
                               vk::raii::SurfaceKHR &surface, IVulkanWindow &window,
                               uint32_t graphicsQueue, uint32_t presentQueue)
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
}

void Swapchain::CreateImageViews(vk::raii::Device &device)
{
    vk::ComponentMapping componentMapping{
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity};

    vk::ImageSubresourceRange subresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    m_imageViews.resize(m_images.size());

    for (auto i = 0; i < m_images.size(); ++i)
    {
        vk::ImageViewCreateInfo createInfo{{},
                                           m_images[0],
                                           vk::ImageViewType::e2D,
                                           m_imageFormat,
                                           componentMapping,
                                           subresourceRange};

        m_imageViews[i] = device.createImageView(createInfo);
    }
}

void Swapchain::CreateFrameBuffers(vk::raii::Device &device, vk::raii::RenderPass &renderPass)
{
    //m_frameBuffers.resize(m_imageViews.size());

    const uint32_t layers = 0;
    for (auto i = 0; i < m_imageViews.size(); ++i)
    {
        vk::FramebufferCreateInfo frameBufferCreateInfo{
            {}, renderPass, m_imageViews[i], CurrentWidth(), CurrentHeight(), layers};
        auto frameBuffer = device.createFramebuffer(frameBufferCreateInfo);
    }
}

} // namespace vksimple