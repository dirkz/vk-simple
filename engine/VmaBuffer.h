#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct VmaBuffer
{
    VmaBuffer(VmaAllocator allocator, VkBuffer buffer, VmaAllocation allocation)
        : m_allocator{allocator}, m_buffer{buffer}, m_allocation{allocation} {};

    VmaBuffer() = delete;
    VmaBuffer(const VmaBuffer &) = delete;

    ~VmaBuffer()
    {
        vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
    };

    VmaBuffer &operator=(const VmaBuffer &) = delete;

  private:
    VmaAllocator m_allocator;
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
};

} // namespace vkdeck
