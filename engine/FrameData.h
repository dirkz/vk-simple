#pragma once

#include "stdafx.h"

#include "Vma.h"

namespace vkdeck
{

struct FrameData
{
    FrameData(vk::raii::Device &device, vk::raii::CommandPool &commandPool, Vma &vma,
              vk::raii::DescriptorSet &descriptorSet);

    FrameData(std::nullptr_t)
    {
    }

    FrameData() = delete;
    FrameData(FrameData &) = delete;

    /// <summary>
    /// For use in `std::vector`.
    /// </summary>
    /// <param name="rhs"></param>
    FrameData(FrameData &&rhs)
    {

        std::swap(m_commandBuffer, rhs.m_commandBuffer);
        std::swap(m_imageAvailableSemaphore, rhs.m_imageAvailableSemaphore);
        std::swap(m_inflightFence, rhs.m_inflightFence);
        std::swap(m_uniformBuffer, rhs.m_uniformBuffer);
        std::swap(m_descriptorSet, rhs.m_descriptorSet);
    }

    FrameData &operator=(FrameData &&rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_commandBuffer, rhs.m_commandBuffer);
            std::swap(m_imageAvailableSemaphore, rhs.m_imageAvailableSemaphore);
            std::swap(m_inflightFence, rhs.m_inflightFence);
            std::swap(m_uniformBuffer, rhs.m_uniformBuffer);
            std::swap(m_descriptorSet, rhs.m_descriptorSet);
        }
        return *this;
    }

    FrameData &operator=(FrameData const &) = delete;

    void RecreateSemaphore(vk::raii::Device &device, vk::raii::Semaphore &semaphore);

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

    VmaBuffer &UniformBuffer()
    {
        return m_uniformBuffer;
    }

  private:
    vk::raii::CommandBuffer m_commandBuffer = nullptr;
    vk::raii::Semaphore m_imageAvailableSemaphore = nullptr;
    vk::raii::Fence m_inflightFence = nullptr;
    VmaBuffer m_uniformBuffer = nullptr;
    vk::raii::DescriptorSet m_descriptorSet = nullptr;
};

} // namespace vkdeck
