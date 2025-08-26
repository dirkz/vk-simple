#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "Vma.h"

namespace vkdeck
{

const std::set<std::string> DesiredExtensions{VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                                              VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
                                              VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
                                              VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
                                              VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
                                              VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                              VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
                                              VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,
                                              "VK_KHR_external_memory_win32"};

std::set<std::string> Vma::DesiredPhysicalDeviceExtensions(vk::raii::PhysicalDevice &physicalDevice)
{
    std::set<std::string> extensions{};

    std::vector<vk::ExtensionProperties> actualExtensionProperties =
        physicalDevice.enumerateDeviceExtensionProperties();

    for (const auto &extensionProperties : actualExtensionProperties)
    {
        std::string extensionName{static_cast<const char *>(extensionProperties.extensionName)};
        if (DesiredExtensions.contains(extensionName))
        {
            extensions.insert(extensionName);
        }
    }

    return extensions;
}

} // namespace vkdeck