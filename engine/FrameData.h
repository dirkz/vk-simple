#pragma once

#include "stdafx.h"

namespace vksimple
{

struct FrameData
{
    FrameData(vk::raii::Device &device, vk::raii::CommandPool &commandPool);

    FrameData(std::nullptr_t)
    {
    }

    FrameData &operator=(FrameData &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_commandBuffer, rhs.m_commandBuffer);
            std::swap(m_imageAvailableSemaphore, rhs.m_imageAvailableSemaphore);
            std::swap(m_inflightFence, rhs.m_inflightFence);
        }
        return *this;
    }

    FrameData() = delete;
    FrameData(FrameData &) = delete;
    FrameData &operator=(FrameData const &) = delete;

    vk::raii::CommandBuffer &CommandBuffer()
    {
        return m_commandBuffer;
    }

    vk::raii::Semaphore &ImageAvailableSemaphore()
    {
        return m_imageAvailableSemaphore;
    }

    vk::raii::Fence &InflightFence()
    {
        return m_inflightFence;
    }

  private:
    vk::raii::CommandBuffer m_commandBuffer = nullptr;
    vk::raii::Semaphore m_imageAvailableSemaphore = nullptr;
    vk::raii::Fence m_inflightFence = nullptr;
};

} // namespace vksimple
