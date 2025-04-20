#pragma once

#include <string>
#include <vector>

#include "Buffer.h"
#include "Core/Window.h"
#include "Guard.h"
#include "Pipeline.h"
#include "Resource.h"
#include "ResourcePool.h"
#include "Stream.h"

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
        Handle<GPUBuffer> CreateBuffer(const BufferDesc& desc) { return m_BufferPool.Create(this, desc); }

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

        VmaAllocator         m_Allocator;
        ResourcePool<GPUBuffer> m_BufferPool;
    };
} // namespace wind