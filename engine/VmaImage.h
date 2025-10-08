#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct VmaImage
{
    VmaImage(VmaAllocator allocator, VkImage image, VmaAllocation allocation)
        : m_allocator{allocator}, m_image{image}, m_allocation{allocation} {};

    VmaImage() = delete;
    VmaImage(const VmaImage &) = delete;

    /// <summary>
    /// For use in `std::vector`.
    /// </summary>
    /// <param name="rhs"></param>
    VmaImage(VmaImage &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_allocator, rhs.m_allocator);
            std::swap(m_image, rhs.m_image);
            std::swap(m_allocation, rhs.m_allocation);
        }
    }

    ~VmaImage()
    {
        if (m_allocation)
        {
            vmaDestroyImage(m_allocator, m_image, m_allocation);
        }
    };

    VmaImage &operator=(VmaImage &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_allocator, rhs.m_allocator);
            std::swap(m_image, rhs.m_image);
            std::swap(m_allocation, rhs.m_allocation);
        }
        return *this;
    }

    VmaImage &operator=(const VmaImage &) = delete;

    vk::Image Image() const
    {
        return m_image;
    }

  private:
    VmaAllocator m_allocator = nullptr;
    VkImage m_image = VK_NULL_HANDLE;
    VmaAllocation m_allocation = nullptr;
};

} // namespace vkdeck
