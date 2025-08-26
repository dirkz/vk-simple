#include "Buffer.h"

namespace vkdeck
{

uint32_t Buffer::FindMemoryType(vk::raii::PhysicalDevice &physicalDevice, uint32_t memoryTypeBits,
                                vk::MemoryPropertyFlags memoryPropertyFlags)
{
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        bool memoryTypeBitsMatch = memoryTypeBits & (1 << i);
        if (!memoryTypeBitsMatch)
        {
            continue;
        }
        vk::MemoryPropertyFlags propertyFlags = memoryProperties.memoryTypes[i].propertyFlags;
        vk::MemoryPropertyFlags propertyFlagsAnded = propertyFlags & memoryPropertyFlags;
        if (propertyFlagsAnded == memoryPropertyFlags)
        {
            return i;
        }
    }

    throw std::runtime_error{"no suitable memory found"};
}

} // namespace vkdeck