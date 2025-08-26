#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct Vma
{
    static std::set<std::string> DesiredPhysicalDeviceExtensions(
        vk::raii::PhysicalDevice &physicalDevice);
};

} // namespace vkdeck
