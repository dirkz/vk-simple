#pragma once

#include "stdafx.h"

namespace vksimple
{

struct QueueFamilyIndices
{
    QueueFamilyIndices();
    QueueFamilyIndices(vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface);

    bool IsComplete();

    uint32_t GraphicsQueue();
    uint32_t PresentQueue();

  private:
    std::optional<uint32_t> m_graphicsFamily;
    std::optional<uint32_t> m_presentFamily;
};

} // namespace vksimple
