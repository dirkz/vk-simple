#pragma once

#include "stdafx.h"

#include "VmaBuffer.h"
#include "VmaHelpers.h"

namespace vkdeck
{

struct Vma
{
    /// <summary>
    /// All extensions that help VMA do its job that the given physical device supports.
    /// </summary>
    /// <param name="physicalDevice">The physical device to check extension support for</param>
    /// <returns></returns>
    static std::set<std::string> DesiredPhysicalDeviceExtensions(
        const vk::raii::PhysicalDevice &physicalDevice);

    /// <summary>
    /// The `VmaAllocatorCreateFlagBits` the given physical device supports
    /// (via available extensions)
    /// </summary>
    /// <param name="physicalDevice">The physical device to check</param>
    /// <returns></returns>
    static VmaAllocatorCreateFlagBits PhysicalDeviceCreateFlagBits(
        const vk::raii::PhysicalDevice &physicalDevice);

    Vma(PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, vk::raii::Instance &instance,
        vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device);

    Vma(std::nullptr_t) {};

    Vma() = delete;
    Vma(const Vma &) = delete;

    ~Vma();

    Vma &operator=(const Vma &) = delete;

    Vma &operator=(Vma &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_allocator, rhs.m_allocator);
        }

        return *this;
    }

    VmaBuffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage,
                           VmaAllocationCreateFlags createFlagBits = {},
                           VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO);

  private:
    VmaAllocator m_allocator = nullptr;
};

} // namespace vkdeck
