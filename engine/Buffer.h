#pragma once

#include "stdafx.h"

namespace vksimple
{

struct Buffer
{
    static uint32_t FindMemoryType(vk::PhysicalDevice &physicalDevice, uint32_t memoryTypeBits,
                                   vk::MemoryPropertyFlags memoryPropertyFlags);
};

} // namespace vksimple
