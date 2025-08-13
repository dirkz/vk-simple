#include "Engine.h"

namespace vksimple
{

Engine::Engine(IVulkanWindow &window) : m_window{window}, m_context{window.GetInstanceProcAddr()}
{
    CreateInstance();
}

void Engine::CreateInstance()
{
    vk::ApplicationInfo appInfo{"vk-simple", VK_MAKE_VERSION(0, 0, 1), "vk-simple",
                                VK_API_VERSION_1_4};

    std::vector<std::string> instanceExtensions = m_window.GetInstanceExtensions();
    std::vector<const char *> extensionNames(instanceExtensions.size());
    std::transform(instanceExtensions.begin(), instanceExtensions.end(), extensionNames.begin(),
                   [](const std::string &s) { return s.c_str(); });

    vk::InstanceCreateInfo createInfo{{}, &appInfo, {}, extensionNames};

    m_instance = m_context.createInstance(createInfo);
}

} // namespace vksimple