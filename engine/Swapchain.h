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

    Swapchain &operator=(Swapchain &&rhs)
    {
        if (this != &rhs)
        {
            std::swap(m_swapchain, rhs.m_swapchain);
            std::swap(m_swapchainImages, rhs.m_swapchainImages);
        }
        return *this;
    }

  private:
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_swapchainImages;
};

} // namespace vksimple
