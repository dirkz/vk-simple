#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct Buffer
{
    static uint32_t FindMemoryType(vk::raii::PhysicalDevice &physicalDevice,
                                   uint32_t memoryTypeBits,
                                   vk::MemoryPropertyFlags memoryPropertyFlags);
};

} // namespace vkdeck
