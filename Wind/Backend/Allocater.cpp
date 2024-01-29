#define VMA_IMPLEMENTATION

#include "Allocator.h"

#include "Device.h"

#include "Core/Log.h"

namespace wind
{
    VkAllocator::VkAllocator(GPUDevice& device)
    {
        VmaAllocatorCreateInfo createInfo {.flags                          = {},
                                           .physicalDevice                 = device.physicalDevice(),
                                           .device                         = device.vkDevice(),
                                           .preferredLargeHeapBlockSize    = 0,
                                           .pAllocationCallbacks           = nullptr,
                                           .pDeviceMemoryCallbacks         = nullptr,
                                           .pHeapSizeLimit                 = nullptr,
                                           .pVulkanFunctions               = nullptr,
                                           .instance                       = device.vkInstance(),
                                           .vulkanApiVersion               = VK_API_VERSION_1_3,
                                           .pTypeExternalMemoryHandleTypes = nullptr};

        vmaCreateAllocator(&createInfo, &m_allocator);
        WIND_CORE_INFO("Create vulkan memory allocator");
    }

    VkAllocator::~VkAllocator() { vmaDestroyAllocator(m_allocator); }

    AllocatedBuffer VkAllocator::AllocateBuffer(const vk::BufferCreateInfo&    bufferCreateInfo,
                                                const VmaAllocationCreateInfo& allocationCreateInfo)
    {
        AllocatedBuffer buffer;
        vmaCreateBuffer(m_allocator,
                        (VkBufferCreateInfo*)&bufferCreateInfo,
                        &allocationCreateInfo,
                        (VkBuffer*)&buffer.buffer,
                        &buffer.allocation,
                        nullptr);
        return buffer;
    }

    void VkAllocator::DestroyBuffer(AllocatedBuffer& buffer)
    {
        vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
    }

    AllocatedImage VkAllocator::AllocateImage(const vk::ImageCreateInfo&     imageCreateInfo,
                                              const VmaAllocationCreateInfo& allocationCreateInfo)
    {
        AllocatedImage image;
        vmaCreateImage(m_allocator,
                       (VkImageCreateInfo*)&imageCreateInfo,
                       &allocationCreateInfo,
                       (VkImage*)&image.image,
                       &image.allocation,
                       nullptr);
        return image;
    }

    void VkAllocator::DestroyImage(AllocatedImage& image)
    {
        vmaDestroyImage(m_allocator, image.image, image.allocation);
    }
} // namespace wind
