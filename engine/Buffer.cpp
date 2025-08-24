#include "Buffer.h"

namespace vksimple
{

uint32_t Buffer::FindMemoryType(vk::raii::PhysicalDevice &physicalDevice, uint32_t memoryTypeBits,
                                vk::MemoryPropertyFlags memoryPropertyFlags)
{
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        vk::MemoryType memoryType = memoryProperties.memoryTypes[i];
        vk::MemoryPropertyFlags propertyFlags = memoryType.propertyFlags;
        uint32_t iShifted = 1 << i;
        bool memoryTypeBitsMatch = memoryTypeBits & iShifted;
        vk::MemoryPropertyFlags propertyFlagsAnded = propertyFlags & memoryPropertyFlags;
        if (memoryTypeBitsMatch && propertyFlagsAnded == memoryPropertyFlags)
        {
            return i;
        }
    }

    throw std::runtime_error{"no suitable memory found"};
}

} // namespace vksimple