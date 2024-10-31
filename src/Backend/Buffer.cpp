#include "Backend/Buffer.h"

#include "Backend/Device.h"

namespace wind
{
    Buffer::Buffer(Device* device, vk::BufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo) :
        Resource(device, Tag::Buffer)
    {
        auto& allocator = device->GetAllocator();

        vmaCreateBuffer(allocator,
                        reinterpret_cast<VkBufferCreateInfo*>(&bufferInfo),
                        &allocInfo,
                        reinterpret_cast<VkBuffer*>(&m_allocateBuffer.buffer),
                        &m_allocateBuffer.allocation,
                        nullptr);
    }

    Buffer::~Buffer()
    {
        auto& allocator = m_device->GetAllocator();
        vmaDestroyBuffer(allocator, m_allocateBuffer.buffer, m_allocateBuffer.allocation);
    }
} // namespace wind