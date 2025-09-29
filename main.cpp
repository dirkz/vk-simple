#include <SDL3/SDL_main.h>

#include "Engine.h"
#include "SDLVulkanWindow.h"

// windows.h gets included because of VK_USE_PLATFORM_WIN32_KHR,
// which we nened for VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME
// (VMA will complain about it missing during runtime even if the value
// is used directly).
// And this clashes with our use of `sdl::CreateWindow`.
#ifdef WIN32
#undef CreateWindow
#endif

using namespace vkdeck;

int SDL_main(int argc, char *argv[])
{
    sdl::Init(SDL_INIT_VIDEO);

    SDL_Window *sdlWindow =
        sdl::CreateWindow("vkdeck", 1280, 800, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    {
        SDLVulkanWindow vulkanWindow = SDLVulkanWindow{sdlWindow};
        Engine engine{vulkanWindow};

        bool done = false;
        while (!done)
        {
            SDL_Event event;

            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_QUIT:
                    done = true;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    engine.WindowResized();
                    break;
                }
            }

            // Do game logic, present a frame, etc.

            engine.DrawFrame();
        }

        // Just in case the GPU was still working on something.
        engine.WaitIdle();
    }

    // Close and destroy the window
    SDL_DestroyWindow(sdlWindow);

    sdl::Quit();

    return 0;
}
