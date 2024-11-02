#include "Graphics/Renderer.h"

#include "Backend/Swapchain.h"
#include "Graphics/PSOCache.h"
#include "Graphics/ShaderCache.h"

#include "RenderGraph/RenderGraph.h"
#include "RenderGraph/RenderGraphID.h"
#include "RenderGraph/RenderGraphTexture.h"

using namespace wind::rg;

namespace wind
{
    Renderer::Renderer(Device* device, const SwapchainCreateInfo& createInfo) : m_device(device)
    {
        m_swapchain = device->CreateResource<Swapchain>(createInfo);
        // create shader cache
        m_shaderCache = std::make_unique<ShaderCache>(device);
        m_shaderCache->Init();
        // create pso cache
        m_psoCache = std::make_unique<PSOCache>(device, m_shaderCache.get());
        m_psoCache->Init();
        // create render graph allocator
        m_rgAllocator = std::make_unique<rg::RenderGraphAllocator>();
    }

    Renderer::~Renderer()
    {
        m_shaderCache->Destroy();
        m_psoCache->Destroy();
    }

    std::unique_ptr<Renderer> Renderer::Craete(Device* device, const SwapchainCreateInfo& createInfo)
    {
        return std::make_unique<Renderer>(device, createInfo);
    }

    Renderer& Renderer::SetViewData(const View& view)
    {
        m_view = &view;
        return *this;
    }

    void Renderer::Sync()
    {
        // sync with the swapchain
    }

    void Renderer::Render()
    {
        // render the scene
        RenderGraph renderGraph(m_rgAllocator.get());

        struct RenderData
        {
            RenderGraphID<RenderGraphTexture> color;
        };

        auto& colorPass = renderGraph.AddPass<RenderData>(
            "ColorPass",
            [&](RenderGraph::Builder& builder, RenderData& renderData) {
                // setup job
                renderData.color =
                    builder.Create<RenderGraphTexture>("Color", RenderGraphTexture::Descriptor {.value = 1});
            },
            [](const ResourceRegistry& registry, RenderData& renderData, CommandStream& stream) {
                // do something
            });

        return;
    }
} // namespace wind