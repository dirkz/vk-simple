#pragma once

#include "stdafx.h"

namespace vksimple
{

struct IVulkanWindow
{
    virtual std::vector<std::string> GetInstanceExtensions() = 0;
};

} // namespace vksimple
