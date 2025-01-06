#pragma once

#include "View.h"
#include "Backend/Device.h"
#include "Backend/Stream.h"
#include "RenderGraph/RGAllocator.h"
#include "Core/DeletionQueue.h"

namespace wind
{
    class Swapchain;
    class PSOCache;
    class ShaderCache;
    class SwapchainCreateInfo;

    struct FrameData
    {
        vk::Semaphore    imageAvailable;
        vk::Semaphore    renderFinished;
        vk::Fence        inFlight;
        DeletionQueue    deletionQueue;
        uint32_t         swapChainImageIndex;
        CommandStreamRef commandStream;
        DeletionQueue    frameDeletionQueue;
    };

    // convert a scene and camera view to a renderable image
    class Renderer
    {
    public:
        Renderer(Device* device, const SwapchainCreateInfo& createInfo);
        ~Renderer();

        static std::unique_ptr<Renderer> Craete(Device* device, const SwapchainCreateInfo& createInfo);

        // sync with the renderer
        void BeginFrame();
        void EndFrame();

        // render the scene in a given view
        void Render(const View& view);

    private:
        void       CreateFrameData();
        FrameData& GetCurrentFrameData();

        Device*                                   m_device; // non owning
        View                                      m_view;
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