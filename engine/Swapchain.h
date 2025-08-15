#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{

struct Swapchain
{
    Swapchain(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device,
              vk::raii::SurfaceKHR &surface, IVulkanWindow &window, uint32_t graphicsQueue,
              uint32_t presentQueue);

    Swapchain(std::nullptr_t)
    {
    }

    Swapchain() = delete;
    Swapchain(Swapchain &) = delete;

    Swapchain(Swapchain &&rhs)
        : m_swapchain{std::move(rhs.m_swapchain)},
          m_swapchainImages{std::move(rhs.m_swapchainImages)}
    {
    }

  private:
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_swapchainImages;
};

} // namespace vksimple
