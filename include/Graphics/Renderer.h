#pragma once

#include <functional>
#include <memory>

#include "View.h"

#include "Backend/Device.h"
#include "RenderGraph/RGAllocator.h"

namespace wind
{
    class Swapchain;
    class PSOCache;
    class ShaderCache;
    class SwapchainCreateInfo;

    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void PushFunction(std::function<void()>&& function) { deletors.push_back(function); }

        void Flush()
        {
            // reverse iterate the deletion queue to execute all the functions
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
            {
                (*it)(); // call functors
            }

            deletors.clear();
        }
    };

    struct FrameData
    {
        vk::Semaphore imageAvailable;
        vk::Semaphore renderFinished;
        vk::Fence     inFlight;
        DeletionQueue deletionQueue;
        uint32_t      swapChainImageIndex;
    };

    // convert a scene and camera view to a renderable image
    class Renderer
    {
    public:
        Renderer(Device* device, const SwapchainCreateInfo& createInfo);
        ~Renderer();

        static std::unique_ptr<Renderer> Craete(Device* device, const SwapchainCreateInfo& createInfo);

        // we need set the view and scene data before rendering
        Renderer& SetViewData(const View& view);

        // sync with the renderer
        void BeginFrame();
        void EndFrame();

        // render the scene in a given view
        void Render();

    private:
        void       CreateFrameData();
        FrameData& GetCurrentFrame();

        Device*                                   m_device; // non owning
        const View*                               m_view;   // non owning
        std::thread::id                           m_renderThread;
        std::shared_ptr<Swapchain>                m_swapchain;
        std::unique_ptr<PSOCache>                 m_psoCache;
        std::unique_ptr<ShaderCache>              m_shaderCache;
        std::unique_ptr<rg::RenderGraphAllocator> m_rgAllocator;
        std::vector<FrameData>                    m_frames;
        DeletionQueue                             m_mainDelelteQueue;
        uint32_t                                  m_frameCounter;
    };
} // namespace wind