#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct Vma
{
    static std::set<std::string> Extensions(vk::raii::Context &context);
};

} // namespace vkdeck
