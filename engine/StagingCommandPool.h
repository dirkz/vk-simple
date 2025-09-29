#pragma once

#include "stdafx.h"

#include "Vma.h"

namespace vkdeck
{

struct StagingCommandPool
{
    StagingCommandPool(vk::raii::Device &device, vk::raii::Queue &queue, Vma &vma,
                       uint32_t queueFamilyIndex);

    StagingCommandPool(const StagingCommandPool &) = delete;
    StagingCommandPool() = delete;

    StagingCommandPool &operator=(const StagingCommandPool &) = delete;

    vk::raii::CommandBuffer &BeginNewCommandBuffer();
    void EndCommandBufferAndSubmit(vk::raii::CommandBuffer &commandBuffer);

    void WaitForFences(vk::raii::Device &device);

    /// <summary>
    /// Copies one buffer into another.
    /// </summary>
    /// <param name="src">The source buffer, which must be still around until `WaitForFences` has
    /// finished</param>
    /// <param name="dst">The destination buffer, which must be still around until `WaitForFences`
    /// has finished</param>
    /// <param name="size">Number of bytes to copy</param>
    void CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

    std::pair<VmaBuffer, VmaBuffer> CreateDeviceBufferFromMemory(
        const void *pData, vk::DeviceSize size, vk::BufferUsageFlags bufferUsage,
        VmaAllocationCreateFlags createFlagBits = {});

    void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout);

  private:
    vk::raii::Device &m_device;
    vk::raii::Queue &m_queue;
    Vma &m_vma;
    vk::raii::CommandPool m_commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_commandBuffers;
    std::vector<vk::raii::Fence> m_fences;
};

} // namespace vkdeck
