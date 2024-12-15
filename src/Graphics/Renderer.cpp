#include "Graphics/Renderer.h"

#include "Backend/Command.h"
#include "Backend/Stream.h"
#include "Backend/Swapchain.h"
#include "Graphics/PSOCache.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/ShaderCache.h"

#include "RenderGraph/PassDesc.h"
#include "RenderGraph/RenderGraph.h"
#include "RenderGraph/RenderGraphID.h"
#include "RenderGraph/RenderGraphTexture.h"

using namespace wind::rg;

namespace wind
{
    Renderer::Renderer(Device* device, const SwapchainCreateInfo& createInfo) : m_device(device)
    {
        // create swapchain
        m_swapchain = device->CreateResource<Swapchain>(createInfo);
        // create shader cache
        m_shaderCache = std::make_unique<ShaderCache>(device);
        m_shaderCache->Init();
        // create pso cache
        m_psoCache = std::make_unique<PSOCache>(device, m_shaderCache.get());
        m_psoCache->Init();
        // create render graph allocator
        m_rgAllocator = std::make_unique<rg::RenderGraphAllocator>(device);

        // create frame data
        CreateFrameData();

        m_frameCounter = 0;
    }

    Renderer::~Renderer()
    {
        m_shaderCache->Destroy();
        m_psoCache->Destroy();
        m_mainDelelteQueue.Flush();
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

    void Renderer::BeginFrame()
    {
        // sync with the swapchain
        auto       vkDevice = m_device->GetDevice();
        FrameData& frame    = GetCurrentFrameData();

        auto result = vkDevice.waitForFences(frame.inFlight, true, UINT64_MAX);
        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to wait for fence");
        }

        // reset the fence
        vkDevice.resetFences(frame.inFlight);

        // acquire the next image
        frame.swapChainImageIndex =
            vkDevice.acquireNextImageKHR(m_swapchain->GetSwapchain(), UINT64_MAX, frame.imageAvailable, nullptr).value;
    }

    void Renderer::EndFrame()
    {
        // sync with the swapchain
        auto       vkDevice = m_device->GetDevice();
        FrameData& frame    = GetCurrentFrameData();

        // submit the command buffer
        m_frameCounter++;
    }

    void Renderer::Render()
    {
        // render the scene
        auto&       currentFrameData = GetCurrentFrameData();
        RenderGraph renderGraph(m_rgAllocator.get(), &currentFrameData);

        struct RenderData
        {
            RenderGraphID<RenderGraphTexture> color;
        };

        RenderGraphTexture backBuffer;
        backBuffer.desc.width  = m_swapchain->GetWidth();
        backBuffer.desc.height = m_swapchain->GetHeight();
        backBuffer.desc.format = m_swapchain->GetFormat();

        backBuffer.view  = m_swapchain->GetImageView(currentFrameData.swapChainImageIndex);
        backBuffer.image = m_swapchain->GetImage(currentFrameData.swapChainImageIndex);

        auto backBufferHandle = renderGraph.Import("BackBuffer", backBuffer.desc, backBuffer);
        // import the import resource
    
        // present pass 
        renderGraph.AddPass<RenderData>(
            "Present",
            [&](RenderGraph::Builder& builder, RenderData& data) {
                data.color = backBufferHandle;
                RenderPassDesc::Descriptor descriptor {
                    .attachments =
                    {
                        .color = {data.color},
                    },
                    .viewPort =
                    {
                        .x        = 0.0f,
                        .y        = 0.0f,
                        .width    = static_cast<float>(m_swapchain->GetWidth()),
                        .height   = static_cast<float>(m_swapchain->GetHeight()),
                        .minDepth = 0.0f,
                        .maxDepth = 1.0f,
                    },
                };

                builder.DeclareRenderPass("Present", descriptor);
            },
            [&](const ResourceRegistry& registry, RenderData& data, CommandStream& stream) {
                auto backBuffer = registry.Get(data.color);
                auto pipeline   = m_psoCache->GetPipeline(PipelineID::Lighting);

                RenderPassNode* passNode = registry.GetPass<RenderPassNode>();
                stream.BeginRendering(passNode->GetRenderingInfo());
                stream.BindPipeline(*pipeline);
            });

        renderGraph.Compile();
        renderGraph.Execute(*currentFrameData.commandStream);
        return;
    }

    void Renderer::CreateFrameData()
    {
        m_frames.resize(config::GetRenderConfig().kmaxFramesInFlight);

        vk::Device vkDevice = m_device->GetDevice();

        for (auto& frame : m_frames)
        {
            frame.imageAvailable = vkDevice.createSemaphore({});
            frame.renderFinished = vkDevice.createSemaphore({});
            frame.inFlight       = vkDevice.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
            frame.commandStream  = m_device->CreateResource<CommandStream>(RenderCommandQueueType::Graphics, StreamMode::eImmdiately);
        }

        // push the deletion queue to the main deletion queue
        m_mainDelelteQueue.PushFunction([this]() {
            vk::Device vkDevice = m_device->GetDevice();

            for (auto& frame : m_frames)
            {
                vkDevice.destroySemaphore(frame.imageAvailable);
                vkDevice.destroySemaphore(frame.renderFinished);
                vkDevice.destroyFence(frame.inFlight);
            }
        });
    }

    FrameData& Renderer::GetCurrentFrameData()
    {
        return m_frames[m_frameCounter % config::GetRenderConfig().kmaxFramesInFlight];
    }
} // namespace wind