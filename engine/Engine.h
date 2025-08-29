#pragma once

#include "stdafx.h"

#include "FrameData.h"
#include "IVulkanWindow.h"
#include "QueueFamilyIndices.h"
#include "StagingCommandPool.h"
#include "Swapchain.h"
#include "Vma.h"

namespace vkdeck
{

constexpr int MaxFramesInFlight = 2;

struct Engine
{
    Engine(IVulkanWindow &window);

    void DrawFrame();
    void WaitIdle();
    void WindowResized();

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
    void CreateVma();
    void CreateSwapchain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();
    void CreateFrameBuffers();
    void CreateCommandPool();

    VmaBuffer CreateVertexBuffer(StagingCommandPool &stagingCommandPool);
    VmaBuffer CreateIndexBuffer(StagingCommandPool &stagingCommandPool);

    void CreateDescriptorPool();
    void CreateDescriptorSets();

    /// <summary>
    ///  Combines:
    ///   CreateCommandBuffers
    ///   CreateSyncObjects
    ///   CreateUniformBuffers
    /// </summary>
    void CreateFrameData();

    void RecreateSwapchain();

    void UpdateUniformBuffer(FrameData &frameData);

    void RecordCommandBuffer(vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex);

    FrameData &CurrentFrameData();

    IVulkanWindow &m_window;

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
    vk::raii::SurfaceKHR m_surface = nullptr;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    QueueFamilyIndices m_queueFamilyIndices;
    vk::raii::Device m_device = nullptr;
    Vma m_vma = nullptr;
    Swapchain m_swapchain = nullptr;
    vk::raii::RenderPass m_renderPass = nullptr;
    vk::raii::DescriptorSetLayout m_descriptorSetLayout = nullptr;
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
    vk::raii::Pipeline m_pipeline = nullptr;
    vk::raii::CommandPool m_commandPool = nullptr;

    VmaBuffer m_vertexBuffer = nullptr;
    VmaBuffer m_indexBuffer = nullptr;

    vk::raii::DescriptorPool m_descriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSet> m_descriptorSets;

    std::vector<FrameData> m_frameData;
    uint32_t m_currentFrame = 0;

    vk::raii::Queue m_graphicsQueue = nullptr;
    vk::raii::Queue m_presentQueue = nullptr;

    bool m_windowResized = false;
};

} // namespace vkdeck
