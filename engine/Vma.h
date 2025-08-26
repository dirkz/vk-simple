#pragma once

#include "stdafx.h"

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
        vk::raii::PhysicalDevice &physicalDevice);

    /// <summary>
    /// The `VmaAllocatorCreateFlagBits` the given physical device supports
    /// (via available extensions)
    /// </summary>
    /// <param name="physicalDevice">The physical device to check</param>
    /// <returns></returns>
    static VmaAllocatorCreateFlagBits CreateFlagBits(vk::raii::PhysicalDevice &physicalDevice);

    Vma(std::nullptr_t) {};

    Vma() = delete;
    Vma(const Vma &) = delete;

    ~Vma();

    Vma &operator=(const Vma &) = delete;

    Vma &operator=(Vma &&rhs)
    {
        if (this != &rhs)
        {
        }

        return *this;
    }

  private:
    VmaAllocator m_allocator = nullptr;
};

} // namespace vkdeck
