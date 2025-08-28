#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct StagingCommandPool
{
    StagingCommandPool(vk::raii::Device &device, uint32_t queueFamilyIndex);
    StagingCommandPool(std::nullptr_t) {};
    StagingCommandPool(const StagingCommandPool &) = delete;
    StagingCommandPool() = delete;

    StagingCommandPool &operator=(const StagingCommandPool &) = delete;

    StagingCommandPool &operator=(StagingCommandPool &&rhs)
    {
        if (this != &rhs)
        {
            std::swap(m_commandPool, rhs.m_commandPool);
        }
        return *this;
    }

  private:
    vk::raii::CommandPool m_commandPool = nullptr;
};

} // namespace vkdeck
