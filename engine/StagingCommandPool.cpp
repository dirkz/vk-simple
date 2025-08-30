#include "StagingCommandPool.h"

namespace vkdeck
{
StagingCommandPool::StagingCommandPool(vk::raii::Device &device, vk::raii::Queue &queue, Vma &vma,
                                       uint32_t queueFamilyIndex)
    : m_device{device}, m_queue{queue}, m_vma{vma}
{

    vk::CommandPoolCreateInfo commandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eTransient,
                                                    queueFamilyIndex};

    m_commandPool = device.createCommandPool(commandPoolCreateInfo);
}

vk::raii::CommandBuffer &StagingCommandPool::BeginNewCommandBuffer()
{
    vk::CommandBufferAllocateInfo allocateInfo{m_commandPool, vk::CommandBufferLevel::ePrimary, 1};
    std::vector<vk::raii::CommandBuffer> commandBuffers =
        m_device.allocateCommandBuffers(allocateInfo);
    m_commandBuffers.push_back(std::move(commandBuffers[0]));

    vk::raii::CommandBuffer &commandBuffer = m_commandBuffers[m_commandBuffers.size() - 1];

    vk::CommandBufferBeginInfo beginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void StagingCommandPool::EndCommandBufferAndSubmit(vk::raii::CommandBuffer &commandBuffer)
{
    commandBuffer.end();

    vk::FenceCreateInfo fenceCreateInfo{};
    vk::raii::Fence fence = m_device.createFence(fenceCreateInfo);

    vk::SubmitInfo submitInfo{{}, {}, *commandBuffer};
    m_queue.submit(submitInfo, fence);
    m_fences.push_back(std::move(fence));
}

void StagingCommandPool::WaitForFences(vk::raii::Device &device)
{
    std::vector<vk::Fence> fences{};
    for (const vk::raii::Fence &fence : m_fences)
    {
        fences.push_back(*fence);
    }

    vk::Result result =
        device.waitForFences(fences, vk::True, std::numeric_limits<uint32_t>::max());
}

void StagingCommandPool::CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{
    vk::raii::CommandBuffer &commandBuffer = BeginNewCommandBuffer();

    vk::BufferCopy bufferCopy{0, 0, size};
    commandBuffer.copyBuffer(src, dst, bufferCopy);

    EndCommandBufferAndSubmit(commandBuffer);
}

std::pair<VmaBuffer, VmaBuffer> StagingCommandPool::StageBuffer(
    const void *pData, vk::DeviceSize size, vk::BufferUsageFlags bufferUsage,
    VmaAllocationCreateFlagBits createFlagBits)
{
    VmaBuffer stagingBuffer =
        m_vma.CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
                           static_cast<VmaAllocationCreateFlagBits>(
                               VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                               VMA_ALLOCATION_CREATE_MAPPED_BIT));

    stagingBuffer.CopyMemoryToAllocation(pData);

    VmaBuffer buffer =
        m_vma.CreateBuffer(size, bufferUsage | vk::BufferUsageFlagBits::eTransferDst);

    CopyBuffer(stagingBuffer.Buffer(), buffer.Buffer(), size);

    return std::make_pair(std::move(buffer), std::move(stagingBuffer));
}

} // namespace vkdeck