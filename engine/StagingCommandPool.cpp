#include "StagingCommandPool.h"

namespace vkdeck
{

StagingCommandPool::StagingCommandPool(vk::raii::Device &device, uint32_t queueFamilyIndex)
{

    vk::CommandPoolCreateInfo commandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eTransient,
                                                    queueFamilyIndex};

    m_commandPool = device.createCommandPool(commandPoolCreateInfo);
}

vk::raii::CommandBuffer &StagingCommandPool::BeginNewCommandBuffer(vk::raii::Device &device)
{
    vk::CommandBufferAllocateInfo allocateInfo{m_commandPool, vk::CommandBufferLevel::ePrimary, 1};
    std::vector<vk::raii::CommandBuffer> commandBuffers =
        device.allocateCommandBuffers(allocateInfo);
    m_commandBuffers.push_back(std::move(commandBuffers[0]));

    vk::raii::CommandBuffer &commandBuffer = m_commandBuffers[0];

    vk::CommandBufferBeginInfo beginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void StagingCommandPool::EndAndSubmitCommandBuffer(vk::raii::Device &device, vk::raii::Queue &queue,
                                                   vk::raii::CommandBuffer &commandBuffer)
{
    commandBuffer.end();

    vk::FenceCreateInfo fenceCreateInfo{};
    vk::Fence fence = device.createFence(fenceCreateInfo);

    vk::SubmitInfo submitInfo{{}, {}, *commandBuffer};
    queue.submit(submitInfo, fence);
    m_fences.push_back(fence);
}

void StagingCommandPool::WaitForFences(vk::raii::Device &device)
{
    vk::Result result =
        device.waitForFences(m_fences, vk::True, std::numeric_limits<uint32_t>::max());
}

} // namespace vkdeck