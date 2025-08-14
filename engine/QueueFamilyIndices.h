#pragma once

#include "stdafx.h"

namespace vksimple
{

struct QueueFamilyIndices
{
    QueueFamilyIndices(vk::raii::PhysicalDevice &device);

  private:
    std::optional<uint32_t> m_graphicsFamily;
};

} // namespace vksimple
