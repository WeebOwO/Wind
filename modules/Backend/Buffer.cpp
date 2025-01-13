#include "Buffer.h"

#include "Device.h"

namespace wind
{
    Buffer::Buffer(Device* device, vk::BufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo) :
        Resource(device, Tag::Buffer), m_AllocationInfo(allocInfo), m_BufferInfo(bufferInfo)
    {}

    Buffer::Buffer(Device*                        device,
                   const std::string&             name,
                   vk::BufferCreateInfo&          bufferInfo,
                   const VmaAllocationCreateInfo& allocInfo) :
        Resource(device, Tag::Buffer), m_AllocationInfo(allocInfo), m_BufferInfo(bufferInfo)
    {
        m_DebugName = name;
    }

    Buffer::Buffer(Device* device, const BufferCreateInfo& createInfo) :
        Resource(device, Tag::Buffer),
        m_BufferInfo({.size        = createInfo.bytesize,
                      .usage       = createInfo.usage,
                      .sharingMode = vk::SharingMode::eExclusive}),
        m_AllocationInfo({.usage = createInfo.memoryUsage}),
        m_MappedData(nullptr)
    {
        m_DebugName = createInfo.debugName;
    }

    void Buffer::InitRHI()
    {
        auto& allocator = m_device->GetAllocator();

        vmaCreateBuffer(allocator,
                        reinterpret_cast<VkBufferCreateInfo*>(&m_BufferInfo),
                        &m_AllocationInfo,
                        reinterpret_cast<VkBuffer*>(&m_AllocateBuffer.buffer),
                        &m_AllocateBuffer.allocation,
                        nullptr);
    };

    void Buffer::ReleaseRHI()
    {
        auto& allocator = m_device->GetAllocator();
        vmaDestroyBuffer(allocator, m_AllocateBuffer.buffer, m_AllocateBuffer.allocation);
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

    Buffer::~Buffer() {}
} // namespace wind