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

std::pair<VmaBuffer, VmaBuffer> StagingCommandPool::CreateDeviceBufferFromMemory(
    const void *pData, vk::DeviceSize size, vk::BufferUsageFlags bufferUsage,
    VmaAllocationCreateFlags createFlagBits)
{
    VmaBuffer stagingBuffer = m_vma.CreateBuffer(
        size, vk::BufferUsageFlagBits::eTransferSrc,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

    stagingBuffer.CopyMemoryToAllocation(pData);

    VmaBuffer buffer =
        m_vma.CreateBuffer(size, bufferUsage | vk::BufferUsageFlagBits::eTransferDst);

    CopyBuffer(stagingBuffer.Buffer(), buffer.Buffer(), size);

    return std::make_pair(std::move(buffer), std::move(stagingBuffer));
}

static bool HasStencilComponent(vk::Format format)
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void StagingCommandPool::TransitionImageLayout(vk::Image image, vk::Format format,
                                               vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::raii::CommandBuffer &commandBuffer = BeginNewCommandBuffer();

    vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;

    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        aspectMask = vk::ImageAspectFlagBits::eDepth;
        if (HasStencilComponent(format))
        {
            aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    }

    constexpr uint32_t baseMipLevel = 0;
    constexpr uint32_t levelCount = 1;
    constexpr uint32_t baseArrayLayer = 0;
    constexpr uint32_t layerCount = 1;
    vk::ImageSubresourceRange imageSubresourceRange{aspectMask, baseMipLevel, levelCount,
                                                    baseArrayLayer, layerCount};

    vk::AccessFlags srcAccessMask{};
    vk::AccessFlags dstAccessMask{};
    vk::PipelineStageFlags srcStageMask{};
    vk::PipelineStageFlags dstStageMask{};

    if (oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        srcAccessMask = {};
        dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStageMask = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
             newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        dstAccessMask = vk::AccessFlagBits::eShaderRead;
        srcStageMask = vk::PipelineStageFlagBits::eTransfer;
        dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined &&
             newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        srcAccessMask = {};
        dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                        vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else
    {
        throw std::runtime_error{"unsupported layout transition"};
    }

    vk::ImageMemoryBarrier imageMemoryBarrier{
        srcAccessMask,          dstAccessMask,          oldLayout, newLayout,
        vk::QueueFamilyIgnored, vk::QueueFamilyIgnored, image,     imageSubresourceRange};

    commandBuffer.pipelineBarrier(srcStageMask, dstStageMask, vk::DependencyFlagBits{}, {}, {},
                                  imageMemoryBarrier);

    EndCommandBufferAndSubmit(commandBuffer);
}

void StagingCommandPool::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width,
                                           uint32_t height)
{
    vk::raii::CommandBuffer &commandBuffer = BeginNewCommandBuffer();

    constexpr vk::DeviceSize bufferOffset = 0;
    constexpr uint32_t bufferRowLength = 0;   // tightly packed
    constexpr uint32_t bufferImageHeight = 0; // tightly packed

    constexpr uint32_t mipLevel = 0;
    constexpr uint32_t baseArrayLayer = 0;
    constexpr uint32_t layerCount = 1;
    vk::ImageSubresourceLayers imageSubresourceLayers{vk::ImageAspectFlagBits::eColor, mipLevel,
                                                      baseArrayLayer, layerCount};

    vk::Offset3D imageOffset{0, 0, 0};
    vk::Extent3D imageExtent{width, height, 1};

    vk::BufferImageCopy bufferImageCopy{bufferOffset,           bufferRowLength, bufferImageHeight,
                                        imageSubresourceLayers, imageOffset,     imageExtent};

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal,
                                    bufferImageCopy);

    EndCommandBufferAndSubmit(commandBuffer);
}

} // namespace vkdeck