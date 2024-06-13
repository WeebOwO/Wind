#pragma once

#include <functional>
#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanGuard.h"
#include "SwapChain.h"

namespace wind
{
    enum class Feature : uint32_t
    {
        // 1.2
        MeshShader,
        RayTracing,
        DynamicRendering,
        Sync2,
    };

    enum class QueueType : uint8_t
    {
        Graphics,
        Present,
        Compute
    };

    // represent the vulkan context
    class Context
    {
    public:
        using surfaceCreateFunc = std::function<vk::SurfaceKHR(vk::Instance)>;

        Context(const std::vector<Feature>& featureList,
                bool                        enableValidationLayers,
                const surfaceCreateFunc&    surfaceFunc);
        ~Context();

        [[nodiscard]] vk::Queue GetQueue(QueueType queueType) const;

        void Submit(std::vector<vk::SubmitInfo>& submitInfos);

        vk::Device GetDevice() { return m_device; }

        std::shared_ptr<class SwapChain> CrateSwapChain(const SwapChainDesc& swapChainDesc);


    private:
        void CreateQueue();

        vk::Instance       m_instance;
        vk::PhysicalDevice m_physicalDevice;
        vk::Device         m_device;
        vk::SurfaceKHR     m_surface;

        struct GPUQueue
        {
            uint32_t  familyIndex {0};
            vk::Queue queue;
        };

        GPUQueue m_graphicsQueue;
        GPUQueue m_presentQueue;
        GPUQueue m_computeQueue;

        std::atomic<uint32_t> m_resourceCount;

        vk::CommandBuffer m_commandBuffer; // use for background task
    };

    void     Init(GLFWwindow* window);
    Context* GetContext();
    void     Quit();
} // namespace wind