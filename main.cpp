#include <SDL3/SDL_main.h>

#include "Engine.h"
#include "SDLVulkanWindow.h"

using namespace vksimple;

int SDL_main(int argc, char *argv[])
{
    sdl::Init(SDL_INIT_VIDEO);

    SDL_Window *sdlWindow = sdl::CreateWindow("vk-simple", 800, 600, SDL_WINDOW_VULKAN);

    SDLVulkanWindow vulkanWindow = SDLVulkanWindow{sdlWindow};
    std::vector<std::string> instanceExtensions = vulkanWindow.GetInstanceExtensions();
    PFN_vkGetInstanceProcAddr instanceProcAddr = vulkanWindow.GetInstanceProcAddr();

    Engine engine{vulkanWindow};

    bool done = false;
    while (!done)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                done = true;
            }
        }

        // Do game logic, present a frame, etc.
    }

    // Close and destroy the window
    SDL_DestroyWindow(sdlWindow);

    sdl::Quit();

    return 0;
}
