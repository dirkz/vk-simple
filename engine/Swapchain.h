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

    Swapchain(std::nullptr_t)
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
        }
        return *this;
    }

  private:
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_images;
    vk::Format m_imageFormat;
    vk::Extent2D m_extent;
};

} // namespace vksimple
