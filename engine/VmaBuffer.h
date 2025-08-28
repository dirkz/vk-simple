#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct VmaBuffer
{
    VmaBuffer(VmaAllocator allocator, VkBuffer buffer, VmaAllocation allocation,
              vk::DeviceSize size)
        : m_allocator{allocator}, m_buffer{buffer}, m_allocation{allocation}, m_size{size} {};

    VmaBuffer() = delete;
    VmaBuffer(const VmaBuffer &) = delete;

    ~VmaBuffer()
    {
        vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
    };

    VmaBuffer &operator=(const VmaBuffer &) = delete;

    VkBuffer Buffer() const
    {
        return m_buffer;
    }

    void CopyMemoryToAllocation(const void *src) const;

  private:
    VmaAllocator m_allocator;
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
    vk::DeviceSize m_size;
};

} // namespace vkdeck
