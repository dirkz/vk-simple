#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{
struct Engine
{
    Engine(IVulkanWindow &window);

  private:
    bool CheckValidationLayerSupport();
    std::vector<std::string> GetRequiredExtensionNames();
    vk::DebugUtilsMessengerCreateInfoEXT CreateDebugUtilsMessengerCreateInfo();
    void CreateInstance();
    void SetupDebugMessenger();

    IVulkanWindow &m_window;

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
};

} // namespace vksimple
