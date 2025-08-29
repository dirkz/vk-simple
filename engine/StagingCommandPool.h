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
    void EndAndSubmitCommandBuffer(vk::raii::Device &device, vk::raii::Queue &queue,
                                   vk::raii::CommandBuffer &commandBuffer);

    void WaitForFences(vk::raii::Device &device);

    void CopyBuffer(vk::raii::Device &device, vk::raii::Queue &queue, vk::Buffer src,
                    vk::Buffer dst, vk::DeviceSize size);

    void CopyBuffer(vk::raii::Device &device, vk::raii::Queue &queue, VmaBuffer *src,
                    vk::Buffer dst, vk::DeviceSize size);

  private:
    vk::raii::CommandPool m_commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_commandBuffers;
    std::vector<vk::raii::Fence> m_fences;
    std::vector<std::unique_ptr<VmaBuffer>> m_stagingBuffers;
};

} // namespace vkdeck
