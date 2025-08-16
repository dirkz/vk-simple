#pragma once

#include "stdafx.h"

namespace vksimple
{

struct ShaderModule
{
    ShaderModule(const std::string &filename);

  private:
    std::vector<char> m_buffer;
};

} // namespace vksimple
