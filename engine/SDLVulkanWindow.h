#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{

inline void HandleSDLError(bool errorCheck, const char *functionName)
{
    if (errorCheck)
    {
        constexpr size_t ErrorMessageSize = 256;
        char errorMsg[ErrorMessageSize];

        const char *sdlErrorMessage = SDL_GetError();
        if (sdlErrorMessage && sdlErrorMessage[0])
        {
            SDL_snprintf(errorMsg, ErrorMessageSize, "SDL error calling %s: %s", functionName,
                         sdlErrorMessage);
        }
        else
        {
            SDL_snprintf(errorMsg, ErrorMessageSize, "SDL error calling %s", functionName);
        }

        SDL_Log("%s", errorMsg);
        throw std::runtime_error{errorMsg};
    }
}

struct SDLVulkanWindow : IVulkanWindow
{
    SDLVulkanWindow(SDL_Window *window) : m_window{window} {};

    std::vector<std::string> GetInstanceExtensions() override;
    PFN_vkGetInstanceProcAddr GetInstanceProcAddr() override;
    vk::raii::SurfaceKHR CreateSurface(vk::raii::Instance &instance) override;
    vk::Extent2D GetExtentInPixels() override;

  private:
    SDL_Window *m_window;
};

} // namespace vksimple
