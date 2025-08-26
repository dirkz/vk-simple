#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct ShaderModuleLoader
{
    ShaderModuleLoader(const std::string &filename);

    vk::raii::ShaderModule CreateShaderModule(vk::raii::Device &device);

  private:
    std::vector<char> m_buffer;
};

} // namespace vkdeck
