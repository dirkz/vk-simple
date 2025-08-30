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

    vk::DeviceSize uniformBufferSize = sizeof(UniformObject);
    m_uniformBuffer = vma.CreateBuffer(uniformBufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                                       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                           VMA_ALLOCATION_CREATE_MAPPED_BIT);

    vk::DescriptorBufferInfo descriptorBufferInfo{m_uniformBuffer.Buffer(), 0, uniformBufferSize};

    vk::WriteDescriptorSet writeDescriptorSet{
        m_descriptorSet, 0, 0, vk::DescriptorType::eUniformBuffer, {}, descriptorBufferInfo};

    device.updateDescriptorSets(writeDescriptorSet, {});
}

void FrameData::RecreateSemaphore(vk::raii::Device &device, vk::raii::Semaphore &semaphore)
{
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    semaphore = device.createSemaphore(semaphoreCreateInfo);
}

} // namespace vkdeck