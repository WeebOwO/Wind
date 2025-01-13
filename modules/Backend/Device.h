#pragma once

#include <queue>
#include <thread>

#include "Buffer.h"
#include "Command.h"
#include "ResourcePool.h"

#include "Core/NonCopy.h"
#include "Core/Vector.h"
#include "Core/Window.h"

namespace wind
{
    class Swapchain;
    class SwapchainCreateInfo;
    class CommandStream;

    struct DeviceExtensions
    {
        wind::vector<std::string> extensions;
        bool                      enableValidationLayers;
    };

    class Device
    {
    public:
        NON_COPYABLE(Device);

        Device(const DeviceExtensions& extensions, Window* window);
        ~Device();

        void                           WaitIdle() { m_Device.waitIdle(); }
        static std::unique_ptr<Device> Create(const DeviceExtensions& extensions, Window* window);

        auto& GetAllocator() { return m_Allocator; }

        // get and set functions
        vk::Instance       GetInstance() { return m_Instance; }
        vk::Device         GetDevice() { return m_Device; }
        vk::SurfaceKHR     GetSurface() { return m_Surface; }
        vk::PhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }

        vk::Queue GetQueue(RenderCommandQueueType queueType)
        {
            return queueType == RenderCommandQueueType::Graphics     ? m_MainQueue.queue :
                   queueType == RenderCommandQueueType::AsyncCompute ? m_AsyncComputeQueue.queue :
                   queueType == RenderCommandQueueType::Copy         ? m_TransferQueue.queue :
                                                                       nullptr;
        }

        // all resources should be created through this function,
        // rathern than directly new from target resource constructor
        template<typename T, typename... Args>
        requires std::is_constructible_v<T, Device*, Args...>&&
                 std::derived_from<T, Resource> [[nodiscard]] std::shared_ptr<T>
                 CreateResourceRef(Args&&... args)
        {
            return std::make_shared<T>(this, std::forward<Args>(args)...);
        }

        uint32_t GetQueueFamilyIndex(RenderCommandQueueType queueType);

        Handle<Buffer> CreateBuffer(const BufferCreateInfo& createInfo);
        void           UpdateBuffer(Handle<Buffer> handle, const void* data, size_t size);

        template<typename T>
        T* GetResource(const Handle<T>& handle)
        {
            if constexpr (std::is_same_v<T, Buffer>)
            {
                return m_BufferPool.Get(handle);
            }
            else
            {
                return nullptr;
            }
        }

    private:
        bool Init();
        void Destroy();
        void ClearResources();

        struct ThreadCommandBuffer
        {
            vk::CommandPool        pool;
            vk::CommandBuffer      buffer;
            vk::Fence              fence;
            vk::Semaphore          semaphore;
            vk::PipelineStageFlags waitStage;
            std::thread::id        threadID;
        };

        struct GPUQueue
        {
            vk::Queue queue       = nullptr;
            int       familyIndex = -1;
        };

        Window*            m_Window;
        vk::Device         m_Device;
        vk::Instance       m_Instance;
        vk::PhysicalDevice m_PhysicalDevice;

        vk::SurfaceKHR             m_Surface;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;

        vk::DispatchLoaderDynamic m_DynamicLoader;
        DeviceExtensions          m_Extensions;

        std::vector<ThreadCommandBuffer> m_CommandBuffers;

        GPUQueue m_MainQueue;         // use for graphics and compute
        GPUQueue m_AsyncComputeQueue; // use for async compute
        GPUQueue m_TransferQueue;     // use for transfer

        std::queue<Resource*> m_DeleteQueue;

        VmaAllocator m_Allocator;

        ResourcePool<Buffer> m_BufferPool;
    };
} // namespace wind