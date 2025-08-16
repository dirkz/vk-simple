#pragma once

#include "stdafx.h"

namespace vksimple
{

struct ShaderModuleLoader
{
    ShaderModuleLoader(const std::string &filename);

    vk::raii::ShaderModule CreateShaderModule(vk::raii::Device &device);

  private:
    std::vector<char> m_buffer;
};

} // namespace vksimple
