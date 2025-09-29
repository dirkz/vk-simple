#include "DebugUtils.h"

namespace vkdeck
{

static VKAPI_ATTR vk::Bool32 VKAPI_CALL
DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              vk::DebugUtilsMessageTypeFlagsEXT messageType,
              const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        const char *kind = "UNKNOWN";
        switch (messageSeverity)
        {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            kind = "VERBOSE";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            kind = "INFO";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            kind = "WARNING";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            kind = "ERROR";
            break;
        }
        SDL_Log("Validation (%s): %s", kind, pCallbackData->pMessage);

        return vk::False;
    }

    return vk::False;
}
vk::DebugUtilsMessengerCreateInfoEXT DebugUtils::CreateDebugUtilsMessengerCreateInfo()
{
    constexpr vk::DebugUtilsMessageSeverityFlagsEXT messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;

    constexpr vk::DebugUtilsMessageTypeFlagsEXT messageType =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{
        {}, messageSeverity, messageType, DebugCallback};

    return createInfo;
}

} // namespace vkdeck