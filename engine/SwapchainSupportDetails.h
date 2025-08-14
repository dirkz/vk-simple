#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{

struct SwapchainSupportDetails
{
    SwapchainSupportDetails(vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface);

    bool IsAdequate();
    vk::SurfaceFormatKHR ChooseSurfaceFormat();
    vk::PresentModeKHR ChoosePresentMode();
    vk::Extent2D ChooseExtent(IVulkanWindow &window) const;
    uint32_t MinImageCount() const;
    uint32_t MaxImageCount() const;

  private:
    vk::SurfaceCapabilitiesKHR m_capabilities;
    std::vector<vk::SurfaceFormatKHR> m_formats;
    std::vector<vk::PresentModeKHR> m_presentModes;
};

} // namespace vksimple
