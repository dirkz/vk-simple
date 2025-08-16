#include "ShaderModule.h"

namespace vksimple
{

ShaderModule::ShaderModule(const std::string &filename)
{
    std::filesystem::path basePath{sdl::GetBasePath()};
    std::filesystem::path filepath = basePath / "shaders" / filename;

    std::ifstream file{filepath, std::ios::ate | std::ios::binary};
    if (!file.is_open())
    {
        std::string filenameString = filepath.string();
        char *string;
        SDL_asprintf(&string, "cannot open file: '%s'", filenameString.c_str());
        std::string message{string};
        throw std::runtime_error{message};
    }
}

} // namespace vksimple