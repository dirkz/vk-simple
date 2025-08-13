#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{
struct Engine
{
    Engine(IVulkanWindow &window);

    bool CheckValidationLayerSupport();
    void CreateInstance();

  private:
    IVulkanWindow &m_window;

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
};

} // namespace vksimple
