#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"
#include "QueueFamilyIndices.h"
#include "Swapchain.h"

namespace vksimple
{
struct Engine
{
    Engine(IVulkanWindow &window);

    void DrawFrame();

  private:
    bool CheckValidationLayerSupport();
    std::vector<std::string> GetRequiredExtensionNames();
    vk::DebugUtilsMessengerCreateInfoEXT CreateDebugUtilsMessengerCreateInfo();
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    bool CheckDeviceExtensionSupport(vk::raii::PhysicalDevice &device);
    bool IsDeviceSuitable(vk::raii::PhysicalDevice &device);
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapchain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    void CreateFrameBuffers();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void CreateSyncObjects();

    void RecordCommandBuffer(vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex);

    IVulkanWindow &m_window;

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
    vk::raii::SurfaceKHR m_surface = nullptr;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    QueueFamilyIndices m_queueFamilyIndices;
    vk::raii::Device m_device = nullptr;
    Swapchain m_swapchain = nullptr;
    vk::raii::RenderPass m_renderPass = nullptr;
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
    vk::raii::Pipeline m_pipeline = nullptr;
    vk::raii::CommandPool m_commandPool = nullptr;
    vk::raii::CommandBuffer m_commandBuffer = nullptr;

    vk::raii::Semaphore m_imageAvailableSemaphore = nullptr;
    vk::raii::Fence m_inflightFence = nullptr;

    vk::raii::Queue m_graphicsQueue = nullptr;
    vk::raii::Queue m_presentQueue = nullptr;
};

} // namespace vksimple
