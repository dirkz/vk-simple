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

    uint32_t CurrentWidth() const
    {
        return m_extent.width;
    }

    uint32_t CurrentHeight() const
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

  private:
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_images;
    vk::Format m_imageFormat;
    vk::Extent2D m_extent;
    std::vector<vk::ImageView> m_imageViews;
};

} // namespace vksimple
