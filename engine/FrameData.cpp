#include "FrameData.h"

#include "UniformObject.h"

namespace vkdeck
{

FrameData::FrameData(vk::raii::Device &device, vk::raii::CommandPool &commandPool, Vma &vma,
                     vk::raii::DescriptorSet &descriptorSet)
    : m_descriptorSet{std::move(descriptorSet)}
{
    constexpr uint32_t commandBufferCount = 1;
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{
        commandPool, vk::CommandBufferLevel::ePrimary, commandBufferCount};

    auto commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);
    m_commandBuffer = std::move(commandBuffers[0]);

    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    m_imageAvailableSemaphore = device.createSemaphore(semaphoreCreateInfo);

    vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};
    m_inflightFence = device.createFence(fenceCreateInfo);

    vk::DeviceSize bufferSize = sizeof(UniformObject);
    m_uniformBuffer = vma.CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                                       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
}

void FrameData::RecreateSemaphore(vk::raii::Device &device, vk::raii::Semaphore &semaphore)
{
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    semaphore = device.createSemaphore(semaphoreCreateInfo);
}

} // namespace vkdeck