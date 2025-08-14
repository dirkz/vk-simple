#include "QueueFamilyIndices.h"

namespace vksimple
{

QueueFamilyIndices::QueueFamilyIndices()
{
}

QueueFamilyIndices::QueueFamilyIndices(vk::raii::PhysicalDevice &device)
{
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
    uint32_t i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            m_graphicsFamily = i;
        }

        if (IsComplete())
        {
            break;
        }

        ++i;
    }
}

bool QueueFamilyIndices::IsComplete()
{
    return m_graphicsFamily.has_value();
}

uint32_t QueueFamilyIndices::GraphicsQueue()
{
    return m_graphicsFamily.value();
}

} // namespace vksimple