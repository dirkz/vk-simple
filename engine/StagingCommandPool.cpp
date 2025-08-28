#include "StagingCommandPool.h"

namespace vkdeck
{

StagingCommandPool::StagingCommandPool(vk::raii::Device &device, uint32_t queueFamilyIndex)
{

    vk::CommandPoolCreateInfo commandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eTransient,
                                                    queueFamilyIndex};

    m_commandPool = device.createCommandPool(commandPoolCreateInfo);
}

} // namespace vkdeck