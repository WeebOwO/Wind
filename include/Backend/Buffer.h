#pragma once

#include "Guard.h"
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
        Buffer(Device* device, vk::DeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        ~Buffer() override;

        vk::Buffer GetBuffer() const { return m_allocateBuffer.buffer; }

    private:
        AllocateBuffer m_allocateBuffer;
        void*          m_mappedData;
    };

    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer(Device* device, vk::DeviceSize size);
        ~VertexBuffer() override;

        void UpLoad(void* data, vk::DeviceSize size);

    private:
    };

    class IndexBuffer : public Buffer
    {};

    class ReadbackBuffer : public Buffer
    {};

    class UniformBuffer : public Buffer
    {};

    class StagingBuffer : public Buffer
    {};

    class ComputeBuffer : public Buffer
    {};

} // namespace wind