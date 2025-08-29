#pragma once

#include "stdafx.h"

#include "VmaBuffer.h"

namespace vkdeck
{

struct StagingCommandPool
{
    StagingCommandPool(vk::raii::Device &device, uint32_t queueFamilyIndex);
    StagingCommandPool(std::nullptr_t) {};
    StagingCommandPool(const StagingCommandPool &) = delete;
    StagingCommandPool() = delete;

    StagingCommandPool &operator=(const StagingCommandPool &) = delete;

    StagingCommandPool &operator=(StagingCommandPool &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_commandPool, rhs.m_commandPool);
        }
        return *this;
    }

    vk::raii::CommandBuffer &BeginNewCommandBuffer(vk::raii::Device &device);
    void EndCommandBufferAndSubmit(vk::raii::Device &device, vk::raii::Queue &queue,
                                   vk::raii::CommandBuffer &commandBuffer);

    void WaitForFences(vk::raii::Device &device);

    /// <summary>
    /// Copies one buffer into another.
    /// </summary>
    /// <param name="device"></param>
    /// <param name="queue"></param>
    /// <param name="src">The source buffer, who must be still around until `WaitForFences` has
    /// finished</param>
    /// <param name="dst">The destination buffer, who must be still around until `WaitForFences` has
    /// finished</param>
    /// <param name="size"></param>
    void CopyBuffer(vk::raii::Device &device, vk::raii::Queue &queue, vk::Buffer src,
                    vk::Buffer dst, vk::DeviceSize size);

    /// <summary>
    /// Copies one buffer into another.
    /// </summary>
    /// <param name="device"></param>
    /// <param name="queue"></param>
    /// <param name="src">The source buffer, who will be taken ownership of.
    /// Don't use it after the call!</param>
    /// <param name="dst">The destination buffer, who must be still around until `WaitForFences` has
    /// finished</param>
    /// <param name="size"></param>
    void CopyBuffer(vk::raii::Device &device, vk::raii::Queue &queue, VmaBuffer &src,
                    vk::Buffer dst, vk::DeviceSize size);

  private:
    vk::raii::CommandPool m_commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_commandBuffers;
    std::vector<vk::raii::Fence> m_fences;
    std::vector<VmaBuffer> m_stagingBuffers;
};

} // namespace vkdeck
