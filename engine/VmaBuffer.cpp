#include "VmaBuffer.h"

#include "VmaHelpers.h"

namespace vkdeck
{

void VmaBuffer::CopyMemoryToAllocation(const void *src) const
{
    VkResult result = vmaCopyMemoryToAllocation(m_allocator, src, m_allocation, 0, m_size);
    CheckResult(result);
}

} // namespace vkdeck