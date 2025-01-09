#include "Buffer.h"

#include "Device.h"

namespace wind
{
    Buffer::Buffer(Device* device, vk::BufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo) :
        Resource(device, Tag::Buffer)
    {
        auto& allocator = device->GetAllocator();

        vmaCreateBuffer(allocator,
                        reinterpret_cast<VkBufferCreateInfo*>(&bufferInfo),
                        &allocInfo,
                        reinterpret_cast<VkBuffer*>(&m_AllocateBuffer.buffer),
                        &m_AllocateBuffer.allocation,
                        nullptr);
    }

    Buffer::~Buffer()
    {
        auto& allocator = m_device->GetAllocator();
        vmaDestroyBuffer(allocator, m_AllocateBuffer.buffer, m_AllocateBuffer.allocation);
    }
} // namespace wind