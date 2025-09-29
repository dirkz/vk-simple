#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct DebugUtils
{
    static vk::DebugUtilsMessengerCreateInfoEXT CreateDebugUtilsMessengerCreateInfo();
};

} // namespace vkdeck
