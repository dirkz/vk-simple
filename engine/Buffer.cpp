#include "Buffer.h"

namespace vksimple
{

uint32_t Buffer::FindMemoryType(vk::raii::PhysicalDevice &physicalDevice, uint32_t memoryTypeBits,
                                vk::MemoryPropertyFlags memoryPropertyFlags)
{
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((memoryTypeBits & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags))
        {
            return i;
        }
    }

    throw std::runtime_error{"no suitable memory found"};
}

} // namespace vksimple