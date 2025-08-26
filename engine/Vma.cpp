#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "Vma.h"

namespace vkdeck
{

std::set<std::string> Vma::Extensions(vk::raii::Context &context)
{
    std::set<std::string> desiredExtensions{VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                                            VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
                                            VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
                                            VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
                                            VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
                                            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                            VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
                                            VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,
                                            "VK_KHR_external_memory_win32"};

    std::set<std::string> extensions{};

    std::vector<vk::ExtensionProperties> actualExtensions =
        context.enumerateInstanceExtensionProperties();
    for (const vk::ExtensionProperties &extensionProperty : actualExtensions)
    {
        std::string extensionName{static_cast<const char *>(extensionProperty.extensionName)};
        if (desiredExtensions.contains(extensionName))
        {
            extensions.insert(extensionName);
        }
    }

    return extensions;
}

} // namespace vkdeck