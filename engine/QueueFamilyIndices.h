#pragma once

#include "stdafx.h"

namespace vksimple
{

struct QueueFamilyIndices
{
    QueueFamilyIndices();
    QueueFamilyIndices(vk::raii::PhysicalDevice &device);

    bool IsComplete();

  private:
    std::optional<uint32_t> m_graphicsFamily;
};

} // namespace vksimple
