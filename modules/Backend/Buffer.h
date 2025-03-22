#pragma once

#include <string>

#include "Guard.h"
#include "Utils.h"
#include "Resource.h"

namespace wind
{
    class Device;

    struct BufferDesc
    {
        std::string          name;
        uint32_t             byteSize;
        vk::BufferUsageFlags usage;
        VmaMemoryUsage       memoryUsage;
        std::vector<uint8_t> initialData;
    };

    struct AllocateBuffer
    {
        vk::Buffer        buffer;
        VmaAllocation     allocation;
        VmaAllocationInfo allocationInfo;
    };

    class GPUBuffer : public Resource
    {
    public:
        GPUBuffer(Device* device, const BufferDesc& desc);
        ~GPUBuffer() override;

        bool Map();
        void Unmap();

        vk::Buffer GetNativeHandle() const { return m_AllocateBuffer.buffer; }
        void       MapMemory();
        void       UnmapMemory();

        void InitRHI() override;
        void ReleaseRHI() override;

        void UpdateData(const void* data, size_t size);

    private:
        vk::BufferCreateInfo    m_BufferCreateInfo;
        VmaAllocationCreateInfo m_AllocationInfo;
        AllocateBuffer          m_AllocateBuffer;
        void*                   m_MappedData = nullptr;
    };
} // namespace wind