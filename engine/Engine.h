#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"
#include "QueueFamilyIndices.h"

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
    bool IsDeviceSuitable(vk::raii::PhysicalDevice &device);
    void PickPhysicalDevice();

    IVulkanWindow &m_window;

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    QueueFamilyIndices m_queueFamilyIndices;
};

} // namespace vksimple
