#pragma once

#include <vk_mem_alloc.h>

#include "Resource.h"

namespace wind
{
    class Device;

    struct AllocateBuffer
    {
        vk::Buffer    buffer;
        VmaAllocation allocation;
    };

    struct BufferCreateInfo
    {
        std::string          debugName;
        size_t               bytesize;
        vk::BufferUsageFlags usage;
        VmaMemoryUsage       memoryUsage;
        uint8_t*             initialData;
    };

    class Buffer : public Resource
    {
    public:
        Buffer(Device* device, vk::BufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo);
        Buffer(Device*                        device,
               const std::string&             name,
               vk::BufferCreateInfo&          bufferInfo,
               const VmaAllocationCreateInfo& allocInfo);

        Buffer(Device* device, const BufferCreateInfo& createInfo);

        ~Buffer() override;

        AllocateBuffer GetBuffer() const { return m_AllocateBuffer; }
        void           Map();
        void           Unmap();

        void UpdateData(const void* data, size_t size);

        void InitRHI() override;
        void ReleaseRHI() override;

        template<typename T>
        uint32_t GetElementCount() const
        {
            return m_BufferInfo.size / sizeof(T);
        }

    private:
        vk::BufferCreateInfo    m_BufferInfo;
        VmaAllocationCreateInfo m_AllocationInfo;
        AllocateBuffer          m_AllocateBuffer;
        void*                   m_MappedData;
    };

    using BufferRef = std::shared_ptr<Buffer>;
} // namespace wind