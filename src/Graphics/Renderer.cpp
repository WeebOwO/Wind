#include "Graphics/Renderer.h"
#include "Backend/Swapchain.h"

namespace wind
{
    Renderer::Renderer(Device* device, const SwapchainCreateInfo& createInfo) : m_device(device)
    {
        m_swapchain   = std::make_shared<Swapchain>(device, createInfo);
        m_psoCache    = std::make_unique<PSOCache>(device);
        m_shaderCache = std::make_unique<ShaderCache>(device);
    }

    Renderer::~Renderer()
    {
        m_psoCache.reset();
        m_shaderCache.reset();
        m_swapchain.reset();
    }

    std::unique_ptr<Renderer> Renderer::Craete(Device* device, const SwapchainCreateInfo& createInfo)
    {
        return std::make_unique<Renderer>(device, createInfo);
    }

    void Renderer::Sync() {}

    void Renderer::Render(Scene& scene, View& view)
    {
        // render the scene
    }
} // namespace wind