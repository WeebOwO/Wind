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

    class Buffer : public Resource
    {
    public:
        Buffer(Device* device, vk::BufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo);
        ~Buffer() override;

        AllocateBuffer GetBuffer() const { return m_AllocateBuffer; }
        void Map();
        void Unmap();

        void UpdateData(const void* data, size_t size);

    private:
        AllocateBuffer m_AllocateBuffer;
        void*          m_MappedData;
    };

    using BufferRef = std::shared_ptr<Buffer>;
} // namespace wind