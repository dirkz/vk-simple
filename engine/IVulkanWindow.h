#pragma once

#include "stdafx.h"

namespace vksimple
{

struct IVulkanWindow
{
    virtual std::vector<std::string> GetInstanceExtensions() = 0;
    virtual PFN_vkGetInstanceProcAddr GetInstanceProcAddr() = 0;
    virtual vk::raii::SurfaceKHR CreateSurface(vk::raii::Instance &instance) = 0;
    virtual vk::Extent2D ExtentInPixels() = 0;
};

} // namespace vksimple
