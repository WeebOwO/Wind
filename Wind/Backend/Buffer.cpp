#include "Buffer.h"

#include "Device.h"

namespace wind
{
    GPUBuffer::GPUBuffer(GPUDevice&                     device,
                         uint32_t                       byteSize,
                         vk::BufferUsageFlags           usageFlags,
                         const VmaAllocationCreateInfo& AllocationCreateInfo) noexcept :
        RHIResource(device),
        m_byteSize(byteSize)
    {
        vk::BufferCreateInfo BufferCreateInfo = {
            .size = byteSize, .usage = usageFlags, .sharingMode = vk::SharingMode::eExclusive};
        m_buffer = device.AllocateBuffer(BufferCreateInfo, AllocationCreateInfo);
    }

    GPUBuffer::~GPUBuffer() { device.DestroyBuffer(m_buffer); }

    UploadBuffer::UploadBuffer(GPUDevice& device, uint32_t byteSize, vk::BufferUsageFlags usageFlags) :
        GPUBuffer(
            device,
            byteSize,
            usageFlags,
            VmaAllocationCreateInfo {
                .flags    = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                .usage    = VMA_MEMORY_USAGE_AUTO,
                .priority = 1.0f,
            })
    {}

    DeviceBuffer::DeviceBuffer(GPUDevice& device, uint32_t byteSize, vk::BufferUsageFlags usageFlags) :
        GPUBuffer(device,
                  byteSize,
                  usageFlags,
                  VmaAllocationCreateInfo {
                      .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                      .usage    = VMA_MEMORY_USAGE_AUTO,
                      .priority = 1.0f,
                  })
    {}

    void* UploadBuffer::MapMemory()
    {
        auto& nativeHandle = device.GetAllocator()->NativeHandle();

        if (m_mapMemory == nullptr)
        {
            void* memory = nullptr;
            vmaMapMemory(nativeHandle, GetAllocatedBuffer().allocation, &memory);
            m_mapMemory = memory;
        }

        return m_mapMemory;
    }

    UploadBuffer::~UploadBuffer()
    {
        if (m_mapMemory != nullptr)
        {
            UnmapMemory();
        }
    }

    void UploadBuffer::UnmapMemory()
    {
        auto& nativeHandle = device.GetAllocator()->NativeHandle();
        vmaUnmapMemory(nativeHandle, GetAllocatedBuffer().allocation);
    }

    void UploadBuffer::WriteData(void* data, uint32_t size, uint32_t offset)
    {
        assert(size + offset <= GetByteSize());
        if (m_mapMemory == nullptr)
            MapMemory();

        uint8_t* memory = (uint8_t*)m_mapMemory;
        memory += offset;

        memcpy(memory, data, size);
    }

    ReadBackBuffer::ReadBackBuffer(GPUDevice& device, uint32_t byteSize, vk::BufferUsageFlags usageFlags) :
        GPUBuffer(device,
                  byteSize,
                  usageFlags,
                  VmaAllocationCreateInfo {
                      .flags    = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                      .usage    = VMA_MEMORY_USAGE_AUTO,
                      .priority = 1.0f,
                  })
    {}

    void* ReadBackBuffer::MapMemory()
    {
        auto& nativeHandle = device.GetAllocator()->NativeHandle();
        if (m_mapMemory == nullptr)
        {
            void* memory = nullptr;
            vmaMapMemory(nativeHandle, GetAllocatedBuffer().allocation, &memory);
            m_mapMemory = (void*)memory;
        }

        return m_mapMemory;
    }

    void ReadBackBuffer::UnmapMemory()
    {
        auto& nativeHandle = device.GetAllocator()->NativeHandle();
        vmaUnmapMemory(nativeHandle, GetAllocatedBuffer().allocation);
    }

    ReadBackBuffer::~ReadBackBuffer()
    {
        if (m_mapMemory != nullptr)
        {
            UnmapMemory();
        }
    }

    PushBuffer::PushBuffer(GPUDevice&           device,
                           uint32_t             buffersize,
                           vk::BufferUsageFlags usage,
                           VmaMemoryUsage       vmaMemoryUsage) :
        GPUBuffer(device, buffersize, usage, VmaAllocationCreateInfo {.usage = vmaMemoryUsage})
    {}

    void PushBuffer::Reset() { m_currentOffset = 0; }
} // namespace wind

namespace wind::utils
{
    uint32_t PadUniformBufferSize(GPUDevice& device, uint32_t originSize)
    {
        auto     limits          = device.GetLimits();
        uint32_t minUboAlignment = limits.minUniformBufferOffsetAlignment;
        uint32_t alignedSize     = originSize;
        if (minUboAlignment > 0)
        {
            alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
        }
        return alignedSize;
    }
} // namespace wind::utils