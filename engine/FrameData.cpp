#include "FrameData.h"

namespace vksimple
{

FrameData::FrameData(vk::raii::Device &device, vk::raii::CommandPool &commandPool)
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
}

void FrameData::RecreateSemaphore(vk::raii::Device &device, vk::raii::Semaphore &semaphore)
{
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    semaphore = device.createSemaphore(semaphoreCreateInfo);
}

} // namespace vksimple