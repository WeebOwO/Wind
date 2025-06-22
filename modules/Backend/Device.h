#pragma once

#include <string>
#include <vector>

#include "Buffer.h"
#include "Descriptor.h"
#include "Core/Window.h"
#include "Guard.h"
#include "Image.h"
#include "Pipeline.h"
#include "Resource.h"
#include "ResourcePool.h"
#include "Stream.h"

#include "Core/Log.h"

namespace wind
{
    struct DeviceExtensions
    {
        std::vector<std::string> extensions;
        bool                     enableValidationLayers;
    };

    struct GPUQueue
    {
        vk::Queue queue       = nullptr;
        int       familyIndex = -1;
    };

    using GPUFunction = std::function<void(vk::CommandBuffer cmdBuffer)>;

    class Device
    {
    public:
        Device(const DeviceExtensions& extensions, Window* window);
        ~Device();

        void WaitIdle() { m_Device.waitIdle(); }

        vk::Instance       GetInstance() { return m_Instance; }
        vk::Device         GetDevice() { return m_Device; }
        vk::SurfaceKHR     GetSurface() { return m_Surface; }
        vk::PhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
        GPUQueue           GetQueue(CommandQueueType queueType);

        static std::unique_ptr<Device> Create(const DeviceExtensions& extensions, Window* window);
        auto&                          GetAllocator() { return m_Allocator; }

        // ptr mode
        template<typename T, typename... Args>
        requires std::is_constructible_v<T, Device*, Args...>&&
                 std::derived_from<T, Resource> [[nodiscard]] std::shared_ptr<T>
                 CreateResourceRef(Args&&... args)
        {
            return std::make_shared<T>(this, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        requires std::is_constructible_v<T, Device*, Args...>&&
                 std::derived_from<T, Resource> [[nodiscard]] std::unique_ptr<T>
                 CreateResourceUnique(Args&&... args)
        {
            return std::make_unique<T>(this, std::forward<Args>(args)...);
        }

        // handle mode
        Handle<GPUBuffer>  Create(const BufferDesc& desc) { return m_BufferPool.Create(this, desc); }
        Handle<GPUTexture> Create(const ImageCreateInfo& imageInfo) { return m_TexturePool.Create(this, imageInfo); }

        template<typename T>
        T* Get(const Handle<T>& handle)
        {
            if constexpr (std::is_same_v<T, GPUBuffer>)
            {
                return m_BufferPool.Get(handle);
            }
            else if constexpr (std::is_same_v<T, GPUTexture>)
            {
                return m_TexturePool.Get(handle);
            }
            else
            {
                WIND_CORE_ERROR("Invalid resource type");
                return nullptr;
            }
        }

        template<typename T>
        void Free(T handle)
        {
            if constexpr (std::is_same_v<T, Handle<GPUBuffer>>)
            {
                m_BufferPool.Free(handle);
            }
            else if constexpr (std::is_same_v<T, Handle<GPUTexture>>)
            {
                m_TexturePool.Free(handle);
            }
        }

        void ExecuteImmediate(GPUFunction&& function)
        {
            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            m_ImmediateCommandBuffer.begin(beginInfo);
            function(m_ImmediateCommandBuffer);
            m_ImmediateCommandBuffer.end();

            vk::SubmitInfo submitInfo     = {};
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers    = &m_ImmediateCommandBuffer;

            m_MainQueue.queue.submit(submitInfo, nullptr);
            m_MainQueue.queue.waitIdle();
        }

        // debug region
        void BeginDebugRegion(vk::CommandBuffer cmdBuffer, const char* pMarkerName, const float* color);
        void EndDebugRegion(vk::CommandBuffer cmdBuffer);

        // need to optimize in the future
        void UploadDataToGPU(const void* data, size_t size, Handle<GPUBuffer> buffer);

    private:
        bool Init();
        void Shutdown();

        Window*                    m_Window;
        vk::Device                 m_Device;
        vk::Instance               m_Instance;
        vk::PhysicalDevice         m_PhysicalDevice;
        vk::SurfaceKHR             m_Surface;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        DeviceExtensions           m_Extensions;

        GPUQueue m_MainQueue;         // use for graphics and compute
        GPUQueue m_AsyncComputeQueue; // use for async compute
        GPUQueue m_TransferQueue;     // use for transfer

        VmaAllocator m_Allocator;

        ResourcePool<GPUBuffer>  m_BufferPool;
        ResourcePool<GPUTexture> m_TexturePool;

        vk::CommandBuffer m_ImmediateCommandBuffer;
        vk::CommandPool   m_ImmediateCommandPool;

        std::unique_ptr<GPUBuffer> m_InternalStagingBuffer;
    };
} // namespace wind