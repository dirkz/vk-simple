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

vk::raii::SurfaceKHR SDLVulkanWindow::CreateSurface(vk::raii::Instance &instance)
{
    VkSurfaceKHR surface;

    bool success = SDL_Vulkan_CreateSurface(m_window, *instance, nullptr, &surface);
    HandleSDLError(success == false, "SDL_Vulkan_CreateSurface");

    return vk::raii::SurfaceKHR{instance, surface};
}

vk::Extent2D SDLVulkanWindow::GetExtentInPixels()
{
    int width, height;
    sdl::GetWindowSizeInPixels(m_window, &width, &height);

    return vk::Extent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

} // namespace vksimple