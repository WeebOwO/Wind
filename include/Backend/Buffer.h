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

        AllocateBuffer GetBuffer() const { return m_allocateBuffer; }

    private:
        AllocateBuffer m_allocateBuffer;
        void*          m_mappedData;
    };
} // namespace wind