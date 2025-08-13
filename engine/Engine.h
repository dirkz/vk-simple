#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{
struct Engine
{
    Engine(IVulkanWindow &window);

  private:
    IVulkanWindow &m_window;
    vk::raii::Context m_context;
};

} // namespace vksimple
