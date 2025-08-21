#pragma once

#include "stdafx.h"

namespace vksimple
{

struct FrameData
{
    FrameData(vk::raii::Device &device, vk::raii::CommandPool &commandPool);

    vk::raii::CommandBuffer &CommandBuffer()
    {
        return m_commandBuffer;
    }

    vk::raii::Semaphore &ImageAvailableSemaphore()
    {
        return m_imageAvailableSemaphore;
    }

    // TODO
    vk::raii::Semaphore &RenderFinishedSemaphore()
    {
        return m_renderFinishedSemaphore;
    }

    vk::raii::Fence &InflightFence()
    {
        return m_inflightFence;
    }

  private:
    vk::raii::CommandBuffer m_commandBuffer = nullptr;
    vk::raii::Semaphore m_imageAvailableSemaphore = nullptr;

    // TODO
    vk::raii::Semaphore m_renderFinishedSemaphore = nullptr;

    vk::raii::Fence m_inflightFence = nullptr;
};

} // namespace vksimple
