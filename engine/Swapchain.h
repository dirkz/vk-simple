#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{

struct Swapchain
{
    Swapchain() = delete;
    Swapchain(Swapchain &) = delete;
    Swapchain &operator=(Swapchain const &) = delete;

    Swapchain(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device,
              vk::raii::SurfaceKHR &surface, IVulkanWindow &window, uint32_t graphicsQueue,
              uint32_t presentQueue);

    Swapchain(std::nullptr_t) : m_imageFormat{vk::Format::eR8G8B8A8Srgb}
    {
    }

    Swapchain &operator=(Swapchain &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_swapchain, rhs.m_swapchain);
            std::swap(m_images, rhs.m_images);
            std::swap(m_imageFormat, rhs.m_imageFormat);
            std::swap(m_extent, rhs.m_extent);
            std::swap(m_imageViews, rhs.m_imageViews);
        }
        return *this;
    }

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
        const float minDepth = 0.f;
        const float maxDepth = 1.f;
        return vk::Viewport{
            0.f, 0.f, static_cast<float>(Width()), static_cast<float>(Width()), minDepth, maxDepth};
    }

  private:
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_images;
    vk::Format m_imageFormat;
    vk::Extent2D m_extent;
    std::vector<vk::raii::ImageView> m_imageViews;
    std::vector<vk::raii::Framebuffer> m_frameBuffers;
};

} // namespace vksimple
