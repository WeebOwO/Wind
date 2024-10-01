#pragma once

#include <memory>

#include "Backend/Device.h"
#include "Backend/Swapchain.h"
#include "PSOCache.h"
#include "Scene/Scene.h"
#include "ShaderCache.h"
#include "View.h"

namespace wind
{
    // convert a scene and camera view to a renderable image
    class Renderer
    {
    public:
        Renderer(Device* device, const SwapchainCreateInfo& createInfo);
        ~Renderer();
        
        static std::unique_ptr<Renderer> Craete(Device* device, const SwapchainCreateInfo& createInfo);

        // sync with the renderer
        void Sync();
        void Render(Scene& scene, View& view);

        void Resize();

    private:
        Device*                      m_device; // non owning
        std::shared_ptr<Swapchain>   m_swapchain;
        std::unique_ptr<PSOCache>    m_psoCache;
        std::unique_ptr<ShaderCache> m_shaderCache;
    };
} // namespace wind