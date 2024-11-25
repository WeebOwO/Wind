#pragma once

#include <optional>
#include <queue>
#include <thread>

#include "Buffer.h"
#include "Command.h"
#include "Resource.h"

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

        static std::unique_ptr<Device> Create(const DeviceExtensions& extensions, Window* window);

        auto& GetAllocator() { return m_allocator; }

        // get and set functions
        vk::Instance       GetInstance() { return m_instance; }
        vk::Device         GetDevice() { return m_device; }
        vk::SurfaceKHR     GetSurface() { return m_surface; }
        vk::PhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }

        // all resources should be created through this function,
        // rathern than directly new from target resource constructor
        template<typename T, typename... Args>
        requires std::is_constructible_v<T, Device*, Args...>&&
                 std::derived_from<T, Resource> [[nodiscard]] std::shared_ptr<T>
                 CreateResource(Args&&... args)
        {
            return std::make_shared<T>(this, std::forward<Args>(args)...);
        }

        uint32_t GetQueueFamilyIndex(RenderCommandQueueType queueType);

    private:
        bool Init();
        void Destroy();

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

        Window*            m_window;
        vk::Device         m_device;
        vk::Instance       m_instance;
        vk::PhysicalDevice m_physicalDevice;

        vk::SurfaceKHR             m_surface;
        vk::DebugUtilsMessengerEXT m_debugMessenger;

        vk::DispatchLoaderDynamic m_dynamicLoader;
        DeviceExtensions          m_extensions;

        std::vector<ThreadCommandBuffer> m_commandBuffers;

        GPUQueue m_mainQueue;         // use for graphics and compute
        GPUQueue m_asyncComputeQueue; // use for async compute
        GPUQueue m_transferQueue;     // use for transfer

        std::queue<Resource*> m_deleteQueue;

        VmaAllocator m_allocator;
    };
} // namespace wind