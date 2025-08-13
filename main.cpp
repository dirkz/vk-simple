#include <SDL3/SDL_main.h>

int SDL_main(int argc, char* argv[])
{
    SDL_Log("Hello, world!");

    sdl::Init(SDL_INIT_VIDEO);

    SDL_Window *window = sdl::CreateWindow("vk-simple", 800, 600, SDL_WINDOW_VULKAN);

    sdl::Quit();

    return 0;
}
