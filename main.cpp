#include <SDL3/SDL_main.h>

#include "Engine.h"
#include "SDLVulkanWindow.h"

using namespace vksimple;

int SDL_main(int argc, char *argv[])
{
    sdl::Init(SDL_INIT_VIDEO);

    SDL_Window *sdlWindow =
        sdl::CreateWindow("vk-simple", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

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
