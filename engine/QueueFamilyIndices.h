#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct QueueFamilyIndices
{
    QueueFamilyIndices();
    QueueFamilyIndices(vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface);

    bool IsComplete()
    {
        return m_graphicsFamily.has_value() && m_presentFamily.has_value();
    }

    uint32_t GraphicsQueue()
    {
        return m_graphicsFamily.value();
    }

    uint32_t PresentQueue()
    {
        return m_presentFamily.value();
    }

  private:
    std::optional<uint32_t> m_graphicsFamily;
    std::optional<uint32_t> m_presentFamily;
};

} // namespace vkdeck
