#include "SDLVulkanWindow.h"

namespace vksimple
{

std::vector<std::string> SDLVulkanWindow::GetInstanceExtensions()
{
    Uint32 numExtensions;
    const char *const *instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&numExtensions);

    std::vector<std::string> result{};
    for (Uint32 i = 0; i < numExtensions; ++i)
    {
        std::string extension{instanceExtensions[i]};
        result.push_back(extension);
    }

    return result;
}

PFN_vkGetInstanceProcAddr SDLVulkanWindow::GetInstanceProcAddr()
{
    return reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
}

} // namespace vksimple