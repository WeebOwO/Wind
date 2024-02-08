#pragma once

#include "std.h"

#include "Allocator.h"
#include "Buffer.h"
#include "Command.h"
#include "Descriptor.h"
#include "RasterShader.h"
#include "Texture.h"
#include "VulkanHeader.h"

namespace wind
{
    class VkAllocator;
    class DeviceBuffer;

    struct QueueIndices
    {
        std::optional<uint32_t> mainQueueIndex;
        std::optional<uint32_t> asyncComputeQueueIndex;
        std::optional<uint32_t> transferQueueIndex;

        bool IsComplete()
        {
            return mainQueueIndex.has_value() && asyncComputeQueueIndex.has_value() && transferQueueIndex.has_value();
        }
    };

    class GPUDevice
    {
    public:
        GPUDevice();
        ~GPUDevice();

        operator vk::Device() { return *m_device; }

        void                     WaitIdle();
        vk::Queue                mainQueue() const noexcept { return m_mainQueue; }
        vk::Queue                asyncComputeQueue() const noexcept { return m_asyncComputeQueue; }
        vk::Queue                transferComputeQueue() const noexcept { return m_transferQueue; };
        QueueIndices             queueIndices() const noexcept { return m_queueIndices; }
        vk::Device               vkDevice() const noexcept { return *m_device; }
        vk::PhysicalDevice       physicalDevice() const noexcept { return m_physicalDevice; }
        vk::Instance             vkInstance() const noexcept { return *m_vkInstance; }
        VkAllocator*             vmallocator() const;
        vk::PhysicalDeviceLimits physicalLimits() const noexcept { return m_limits; }
        DeviceBufferRef          CreateDeviceBuffer(uint32_t byteSize, vk::BufferUsageFlags usageFlags);
        UploadBufferRef          CreateUploadBuffer(uint32_t byteSize);
        RasterShaderRef          CreateRastShader(const std::string& debugName,
                                                  const std::string& vertexFilePath,
                                                  const std::string& fragfilePath);
        GPUTextureRef            CreateGPUTexture(const vk::ImageCreateInfo& createInfo);
        vk::DescriptorSet        AllocateDescriptor(const vk::DescriptorSetLayout&) const;
        void                     ExecuteImmediately(const std::function<void(vk::CommandBuffer cb)>& func) const;

    private:
        friend class GPUBuffer;
        friend class GPUTexture;

        void InitAllocator();
        void CreateInstance();
        void PickupPhysicalDevice();
        void CreateDevice();
        void QueryQueueFamilyIndices();
        void InitBackupCommandBuffer();

        AllocatedBuffer AllocateBuffer(const vk::BufferCreateInfo&    bufferCreateInfo,
                                       const VmaAllocationCreateInfo& allocationCreateInfo) const;

        void DestroyBuffer(AllocatedBuffer& buffer) const;

        AllocatedImage AllocateImage(const vk::ImageCreateInfo&     imageCreateInfo,
                                     const VmaAllocationCreateInfo& allocationCreateInfo) const;

        void DestroyImage(AllocatedImage& image) const;

        std::vector<const char*> GetRequiredExtensions();

        uint32_t     m_alignSize;
        QueueIndices m_queueIndices;

        vk::Queue m_mainQueue;         // can perform the general task (graphics, compute, copy)
        vk::Queue m_asyncComputeQueue; // perform the async compute task
        vk::Queue m_transferQueue;     // transfer queue

        vk::UniqueInstance       m_vkInstance;
        vk::PhysicalDevice       m_physicalDevice;
        vk::PhysicalDeviceLimits m_limits;
        vk::UniqueDevice         m_device;

        vk::UniqueDebugUtilsMessengerEXT m_dubugMessenger;
        vk::DynamicLoader                m_vkLoader;

        std::unordered_set<std::string> m_supportedExtensions;
        std::vector<const char*>        m_enableExtensions;

        Scope<VkAllocator> m_allocator;
        bool               m_enableDebug {true};

        // main queue backup command buffer which do some immediate task (in block way, means cpu will stall to sync
        // with gpu)
        vk::CommandPool   m_backupCommandPool;
        vk::CommandBuffer m_backupCommandBuffer;
        vk::Fence         m_backupCommandfence;
    };
} // namespace wind
