#include "Buffer.h"

#include "Core/Log.h"
#include "Device.h"


namespace wind
{
    Buffer::Buffer(Device* device, const BufferDesc& desc) : Resource(device)
    {
        m_BufferCreateInfo = {
            .size        = desc.byteSize,
            .usage       = desc.usage,
            .sharingMode = vk::SharingMode::eExclusive,
        };

        m_DebugName = desc.name;

        m_AllocationInfo = {
            .usage = desc.memoryUsage,
        };
    }

    Buffer::~Buffer() = default;

    void Buffer::InitRHI()
    {
        auto& allocator = m_Device->GetAllocator();

        vmaCreateBuffer(allocator,
                        reinterpret_cast<VkBufferCreateInfo*>(&m_BufferCreateInfo),
                        &m_AllocationInfo,
                        reinterpret_cast<VkBuffer*>(&m_AllocateBuffer.buffer),
                        &m_AllocateBuffer.allocation,
                        nullptr);
    }

    void Buffer::ReleaseRHI()
    {
        vmaDestroyBuffer(m_Device->GetAllocator(), m_AllocateBuffer.buffer, m_AllocateBuffer.allocation);
    }

    bool Buffer::Map()
    {
        vmaMapMemory(m_Device->GetAllocator(), m_AllocateBuffer.allocation, &m_MappedData);
        return true;
    }

    void Buffer::Unmap() { vmaUnmapMemory(m_Device->GetAllocator(), m_AllocateBuffer.allocation); }

    void Buffer::UpdateData(const void* data, size_t size)
    {
        if (m_MappedData == nullptr)
        {
            WIND_CORE_ERROR("Buffer is not mapped");
            return;
        }
        memcpy(m_MappedData, data, size);
    }

} // namespace wind