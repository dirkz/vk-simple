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
    return m_commandBuffers[0];
}

} // namespace vkdeck