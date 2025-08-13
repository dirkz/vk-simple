#pragma once

#include "stdafx.h"

#include "IVulkanWindow.h"

namespace vksimple
{

struct SDLVulkanWindow : IVulkanWindow
{
    SDLVulkanWindow(SDL_Window *window) : m_window{window} {};

    std::vector<std::string> GetInstanceExtensions() override;
    PFN_vkGetInstanceProcAddr GetInstanceProcAddr() override;

  private:
    SDL_Window *m_window;
};

} // namespace vksimple
