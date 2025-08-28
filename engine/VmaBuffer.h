#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct VmaBuffer
{
    VmaBuffer(VmaAllocator allocator, VkBuffer buffer, VmaAllocation allocation,
              vk::DeviceSize size)
        : m_allocator{allocator}, m_buffer{buffer}, m_allocation{allocation}, m_size{size} {};

    VmaBuffer(std::nullptr_t) {};

    VmaBuffer() = delete;
    VmaBuffer(const VmaBuffer &) = delete;

    ~VmaBuffer()
    {
        if (m_allocation)
        {
            vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
        }
    };

    VmaBuffer &operator=(const VmaBuffer &) = delete;

    VmaBuffer &operator=(VmaBuffer &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_allocator, rhs.m_allocator);
            std::swap(m_buffer, rhs.m_buffer);
            std::swap(m_allocation, rhs.m_allocation);
            std::swap(m_size, rhs.m_size);
        }
        return *this;
    }

    vk::Buffer Buffer() const
    {
        return m_buffer;
    }

    void CopyMemoryToAllocation(const void *src) const;

  private:
    VmaAllocator m_allocator = nullptr;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VmaAllocation m_allocation = nullptr;
    vk::DeviceSize m_size = 0;
};

} // namespace vkdeck
