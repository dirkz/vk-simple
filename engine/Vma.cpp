#include "Vma.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "Vma.h"

namespace vkdeck
{

const std::unordered_map<std::string, VmaAllocatorCreateFlagBits> DesiredExtensionFlags{
    std::make_pair(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                   VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT),
    std::make_pair(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT),

    std::make_pair(VK_KHR_MAINTENANCE_4_EXTENSION_NAME, VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT),
    std::make_pair(VK_KHR_MAINTENANCE_5_EXTENSION_NAME, VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT),
    std::make_pair(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME, VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT),
    std::make_pair(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                   VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT),
    std::make_pair(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
                   VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT),
    std::make_pair(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,
                   VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT),
#ifdef WIN32
    std::make_pair(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
                   VMA_ALLOCATOR_CREATE_KHR_EXTERNAL_MEMORY_WIN32_BIT)
#endif
};

std::set<std::string> Vma::DesiredPhysicalDeviceExtensions(
    const vk::raii::PhysicalDevice &physicalDevice)
{
    std::set<std::string> extensions{};

    std::vector<vk::ExtensionProperties> actualExtensionProperties =
        physicalDevice.enumerateDeviceExtensionProperties();

    for (const auto &extensionProperties : actualExtensionProperties)
    {
        std::string extensionName{static_cast<const char *>(extensionProperties.extensionName)};
        if (DesiredExtensionFlags.count(extensionName) > 0)
        {
            extensions.insert(extensionName);
        }
    }

    return extensions;
}

VmaAllocatorCreateFlagBits Vma::CreateFlagBits(const vk::raii::PhysicalDevice &physicalDevice)
{
    VmaAllocatorCreateFlagBits flags{};

    std::vector<vk::ExtensionProperties> actualExtensionProperties =
        physicalDevice.enumerateDeviceExtensionProperties();

    for (const auto &extensionProperties : actualExtensionProperties)
    {
        std::string extensionName{static_cast<const char *>(extensionProperties.extensionName)};
        const auto &it = DesiredExtensionFlags.find(extensionName);
        if (it != DesiredExtensionFlags.end())
        {
            const auto &pair = *it;
            VmaAllocatorCreateFlagBits flagsToAdd = pair.second;
            auto newFlags = flags | flagsToAdd;
            flags = static_cast<VmaAllocatorCreateFlagBits>(newFlags);
        }
    }

    return flags;
}

Vma::Vma(PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, vk::raii::Instance &instance,
         vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device)
{
    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(
        *vkGetInstanceProcAddr(*instance, "vkGetDeviceProcAddr"));

    VmaVulkanFunctions vulkanFunctions{};
    vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateFlagBits flags = CreateFlagBits(physicalDevice);
    VmaAllocatorCreateInfo allocatorCreateInfo{};
    allocatorCreateInfo.flags = flags;
    allocatorCreateInfo.vulkanApiVersion = vk::ApiVersion14;
    allocatorCreateInfo.instance = *instance;
    allocatorCreateInfo.physicalDevice = *physicalDevice;
    allocatorCreateInfo.device = *device;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);
}

Vma::~Vma()
{
    if (m_allocator)
    {
        vmaDestroyAllocator(m_allocator);
    }
}

VmaBuffer Vma::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage,
                            VmaMemoryUsage memoryUsage)
{
    vk::BufferCreateInfo bufferCreateInfo1{{}, size, bufferUsage, vk::SharingMode::eExclusive};
    VkBufferCreateInfo bufferCreateInfo = bufferCreateInfo1;

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = memoryUsage;

    VkBuffer buffer;
    VmaAllocation allocation;
    vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation,
                    nullptr);

    return VmaBuffer{m_allocator, buffer, allocation};
}

} // namespace vkdeck