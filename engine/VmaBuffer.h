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

    /// <summary>
    /// For use in `std::vector`.
    /// </summary>
    /// <param name="rhs"></param>
    VmaBuffer(VmaBuffer &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_allocator, rhs.m_allocator);
            std::swap(m_buffer, rhs.m_buffer);
            std::swap(m_allocation, rhs.m_allocation);
            std::swap(m_size, rhs.m_size);
        }
    }

    ~VmaBuffer()
    {
        if (m_allocation)
        {
            vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
        }
    };

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

    VmaBuffer &operator=(const VmaBuffer &) = delete;

    vk::Buffer Buffer() const
    {
        return m_buffer;
    }

    vk::DeviceSize Size() const
    {
        return m_size;
    }

    void CopyMemoryToAllocation(const void *src) const;

  private:
    VmaAllocator m_allocator = nullptr;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VmaAllocation m_allocation = nullptr;
    vk::DeviceSize m_size = 0;
};

} // namespace vkdeck
