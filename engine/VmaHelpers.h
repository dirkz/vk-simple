#pragma once

namespace vkdeck
{

inline void CheckResult(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        const char *resultString = string_VkResult(result);
        SDL_Log("VkResult error: %s", resultString);
        throw std::runtime_error{std::string{resultString}};
    }
}

} // namespace vkdeck
