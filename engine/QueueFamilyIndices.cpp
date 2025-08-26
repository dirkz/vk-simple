#include "QueueFamilyIndices.h"

namespace vkdeck
{

QueueFamilyIndices::QueueFamilyIndices()
{
}

QueueFamilyIndices::QueueFamilyIndices(vk::raii::PhysicalDevice &device,
                                       vk::raii::SurfaceKHR &surface)
{
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
    uint32_t i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            m_graphicsFamily = i;
        }

        vk::Bool32 isSupported = device.getSurfaceSupportKHR(i, surface);
        if (isSupported)
        {
            m_presentFamily = i;
        }

        if (IsComplete())
        {
            break;
        }

        ++i;
    }
}

} // namespace vkdeck