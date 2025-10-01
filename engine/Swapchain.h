#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vkdeck
{

struct Swapchain
{
    Swapchain(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device,
              vk::raii::SurfaceKHR &surface, IVulkanWindow &window, uint32_t graphicsQueue,
              uint32_t presentQueue);

    Swapchain(std::nullptr_t) : m_imageFormat{vk::Format::eR8G8B8A8Srgb}
    {
    }

    Swapchain() = delete;
    Swapchain(Swapchain &) = delete;

    Swapchain &operator=(Swapchain const &) = delete;

    Swapchain &operator=(Swapchain &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_swapchain, rhs.m_swapchain);
            std::swap(m_images, rhs.m_images);
            std::swap(m_imageFormat, rhs.m_imageFormat);
            std::swap(m_extent, rhs.m_extent);
            std::swap(m_imageViews, rhs.m_imageViews);
            std::swap(m_renderFinishedSemaphores, rhs.m_renderFinishedSemaphores);
        }
        return *this;
    }

    static vk::raii::ImageView CreateImageView(vk::raii::Device &device, vk::Image image,
                                               vk::Format format, vk::ImageAspectFlags aspectFlags);
    void CreateImageViews(vk::raii::Device &device);
    void CreateFrameBuffers(vk::raii::Device &device, vk::raii::RenderPass &renderPass);

    uint32_t Width() const
    {
        return m_extent.width;
    }

    uint32_t Height() const
    {
        return m_extent.height;
    }

    vk::Extent2D Extent() const
    {
        return m_extent;
    }

    vk::Format Format() const
    {
        return m_imageFormat;
    }

    vk::raii::Framebuffer &FrameBufferAt(uint32_t index)
    {
        return m_frameBuffers[index];
    }

    vk::Viewport Viewport() const
    {
        constexpr float x = 0.f;
        const float y = static_cast<float>(Height());
        const float width = static_cast<float>(Width());
        const float height = -static_cast<float>(Height());
        constexpr float minDepth = 0.f;
        constexpr float maxDepth = 1.f;
        return vk::Viewport{x, y, width, height, minDepth, maxDepth};
    }

    vk::Rect2D ScissorRect() const
    {
        return vk::Rect2D{{0, 0}, Extent()};
    }

    std::pair<vk::Result, uint32_t> AcquireNextImage(vk::raii::Semaphore &imageAvailableSemaphore);

    vk::raii::SwapchainKHR &SwapchainKHR()
    {
        return m_swapchain;
    }

    vk::raii::Semaphore &RenderFinishedSemaphoreAt(uint32_t imageIndex)
    {
        return m_renderFinishedSemaphores[imageIndex];
    }

    float Ratio() const
    {
        return static_cast<float>(m_extent.width) / m_extent.height;
    }

  private:
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_images;
    vk::Format m_imageFormat;
    vk::Extent2D m_extent;
    std::vector<vk::raii::ImageView> m_imageViews;
    std::vector<vk::raii::Framebuffer> m_frameBuffers;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
};

} // namespace vkdeck
