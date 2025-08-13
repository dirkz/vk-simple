#include <SDL3/SDL_main.h>

int SDL_main(int argc, char *argv[])
{
    SDL_Log("Hello, world!");

    sdl::Init(SDL_INIT_VIDEO);

    SDL_Window *window = sdl::CreateWindow("vk-simple", 800, 600, SDL_WINDOW_VULKAN);

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
    SDL_DestroyWindow(window);

    sdl::Quit();

    return 0;
}
