#include "SwapchainSupportDetails.h"

namespace vksimple
{

SwapchainSupportDetails::SwapchainSupportDetails(vk::raii::PhysicalDevice &device,
                                                 vk::raii::SurfaceKHR &surface)
{
    m_capabilities = device.getSurfaceCapabilitiesKHR(surface);
    m_formats = device.getSurfaceFormatsKHR(surface);
    m_presentModes = device.getSurfacePresentModesKHR(surface);
}
bool SwapchainSupportDetails::IsAdequate()
{
    return !m_formats.empty() && !m_presentModes.empty();
}

} // namespace vksimple