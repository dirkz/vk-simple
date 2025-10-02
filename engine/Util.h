#pragma once

#include "stdafx.h"

namespace vkdeck
{

inline bool HasStencilComponent(vk::Format format)
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

} // namespace vkdeck
