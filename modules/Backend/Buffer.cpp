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

    void Buffer::Map()
    {
        auto& allocator = m_device->GetAllocator();
        vmaMapMemory(allocator, m_AllocateBuffer.allocation, &m_MappedData);
    }

    void Buffer::Unmap()
    {
        auto& allocator = m_device->GetAllocator();
        vmaUnmapMemory(allocator, m_AllocateBuffer.allocation);
    }

    void Buffer::UpdateData(const void* data, size_t size)
    {
        Map();
        memcpy(m_MappedData, data, size);
        Unmap();
    }

    Buffer::~Buffer()
    {
        m_device->WaitIdle();
        auto& allocator = m_device->GetAllocator();
        vmaDestroyBuffer(allocator, m_AllocateBuffer.buffer, m_AllocateBuffer.allocation);
    }
} // namespace wind