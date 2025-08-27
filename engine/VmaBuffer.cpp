#include "VmaBuffer.h"

namespace vkdeck
{

void VmaBuffer::CopyMemoryToAllocation(const void *src) const
{
    vmaCopyMemoryToAllocation(m_allocator, src, m_allocation, 0, m_size);
}

} // namespace vkdeck