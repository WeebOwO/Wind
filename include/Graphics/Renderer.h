#pragma once

#include <memory>

#include "Backend/Device.h"
#include "View.h"

namespace wind
{
    class Swapchain;
    class PSOCache;
    class ShaderCache;
    class SwapchainCreateInfo;

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
        void Sync();

        // render the scene in a given view
        void Render();

    private:
        Device*                      m_device; // non owning
        const View*                  m_view;   // non owning
        std::shared_ptr<Swapchain>   m_swapchain;
        std::unique_ptr<PSOCache>    m_psoCache;
        std::unique_ptr<ShaderCache> m_shaderCache;
    };
} // namespace wind