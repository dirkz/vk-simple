#include <SDL3/SDL_main.h>

int SDL_main(int argc, char* argv[])
{
    SDL_Log("Hello, world!");

    sdl::Init(SDL_INIT_VIDEO);
    sdl::Quit();

    return 0;
}
