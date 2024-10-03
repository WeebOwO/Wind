#pragma once

#include <queue>
#include <thread>

#include "Guard.h"
#include "Pipeline.h"
#include "Resource.h"
#include "Shader.h"

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

        // get and set functions
        vk::Instance       GetInstance() { return m_instance; }
        vk::Device         GetDevice() { return m_device; }
        vk::SurfaceKHR     GetSurface() { return m_surface; }
        vk::PhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }

        // some native create functions
        std::shared_ptr<Swapchain>        CreateSwapchain(const SwapchainCreateInfo& createInfo);
        std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline();

        void ExecuteStream(CommandStream* stream);

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

        template<typename T, typename... Args>
        [[nodiscard]] std::shared_ptr<T> CreateResource(
            Args&&... args) requires std::is_constructible_v<T, Device*, Args...>&& std::derived_from<T, Resource>
        {
            return std::make_shared<T>(this, std::forward<Args>(args)...);
        }

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
    };
} // namespace wind