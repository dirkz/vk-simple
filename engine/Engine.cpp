#include "Engine.h"

namespace vksimple
{

Engine::Engine(IVulkanWindow &window) : m_window{window}, m_context{window.GetInstanceProcAddr()}
{
}

} // namespace vksimple