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

void StagingCommandPool::EndCommandBufferAndSubmit(vk::raii::Device &device, vk::raii::Queue &queue,
                                                   vk::raii::CommandBuffer &commandBuffer)
{
    commandBuffer.end();

    vk::FenceCreateInfo fenceCreateInfo{};
    vk::raii::Fence fence = device.createFence(fenceCreateInfo);

    vk::SubmitInfo submitInfo{{}, {}, *commandBuffer};
    queue.submit(submitInfo, fence);
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

void StagingCommandPool::CopyBuffer(vk::raii::Device &device, vk::raii::Queue &queue,
                                    vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{
    vk::raii::CommandBuffer &commandBuffer = BeginNewCommandBuffer(device);

    vk::BufferCopy bufferCopy{0, 0, size};
    commandBuffer.copyBuffer(src, dst, bufferCopy);

    EndCommandBufferAndSubmit(device, queue, commandBuffer);
}

void StagingCommandPool::CopyBuffer(vk::raii::Device &device, vk::raii::Queue &queue,
                                    VmaBuffer &src, vk::Buffer dst)
{
    vk::Buffer srcBuffer = src.Buffer();
    CopyBuffer(device, queue, srcBuffer, dst, src.Size());
    m_stagingBuffers.push_back(std::move(src));
}

VmaBuffer StagingCommandPool::StageBuffer(vk::raii::Device &device, vk::raii::Queue &queue,
                                          Vma &vma, const void *pData, vk::DeviceSize size,
                                          vk::BufferUsageFlags bufferUsage,
                                          VmaAllocationCreateFlagBits createFlagBits)
{
    VmaBuffer stagingBuffer =
        vma.CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
                         VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    stagingBuffer.CopyMemoryToAllocation(pData);

    VmaBuffer buffer = vma.CreateBuffer(size, bufferUsage | vk::BufferUsageFlagBits::eTransferDst);

    CopyBuffer(device, queue, stagingBuffer, buffer.Buffer());

    return buffer;
}

} // namespace vkdeck