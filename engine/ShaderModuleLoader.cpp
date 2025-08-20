#include "ShaderModuleLoader.h"

namespace vksimple
{

ShaderModuleLoader::ShaderModuleLoader(const std::string &filename)
{
    std::filesystem::path basePath{sdl::GetBasePath()};
    std::filesystem::path filepath = basePath / "shaders" / filename;

    std::ifstream file{filepath, std::ios::ate | std::ios::binary};

    if (!file.is_open())
    {
        std::string filenameString = filepath.string();
        char *string;
        SDL_asprintf(&string, "cannot open file: '%s'", filenameString.c_str());
        SDL_Log("%s", string);
        std::string message{string};
        SDL_free(string);
        throw std::runtime_error{message};
    }

    size_t fileSize = file.tellg();
    m_buffer.resize(fileSize);

    file.seekg(0);
    file.read(m_buffer.data(), fileSize);

    file.close();
}

vk::raii::ShaderModule ShaderModuleLoader::CreateShaderModule(vk::raii::Device &device)
{
    vk::ShaderModuleCreateInfo createInfo{
        {}, m_buffer.size(), reinterpret_cast<uint32_t *>(m_buffer.data())};
    return device.createShaderModule(createInfo);
}

} // namespace vksimple