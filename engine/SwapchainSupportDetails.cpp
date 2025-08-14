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

vk::SurfaceFormatKHR SwapchainSupportDetails::ChooseSurfaceFormat()
{
    for (const auto &format : m_formats)
    {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return format;
        }
    }

    return m_formats[0];
}

} // namespace vksimple