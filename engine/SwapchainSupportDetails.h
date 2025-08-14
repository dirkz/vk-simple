#pragma once

#include "stdafx.h"

namespace vksimple
{

struct SwapchainSupportDetails
{
    SwapchainSupportDetails(vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface);

    bool IsAdequate();
    vk::SurfaceFormatKHR ChooseSurfaceFormat();
    vk::PresentModeKHR ChoosePresentMode();
    vk::Extent2D ChooseExtent();

  private:
    vk::SurfaceCapabilitiesKHR m_capabilities;
    std::vector<vk::SurfaceFormatKHR> m_formats;
    std::vector<vk::PresentModeKHR> m_presentModes;
};

} // namespace vksimple
