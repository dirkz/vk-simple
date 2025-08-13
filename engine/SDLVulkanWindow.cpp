#include "SDLVulkanWindow.h"

namespace vksimple
{

std::vector<std::string> SDLVulkanWindow::GetInstanceExtensions()
{
    Uint32 numExtensions;
    const char *const *instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&numExtensions);

    std::vector<std::string> result{};
    for (auto i = 0; i < numExtensions; ++i)
    {
        std::string extension{instanceExtensions[i]};
        result.push_back(extension);
    }

    return result;
}

} // namespace vksimple