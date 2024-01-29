#include "RenderGraph.h"

#include "PassNode.h"
#include "ResourceRegistry.h"

#include "Core/Log.h"

#include "Backend/Texture.h"
#include "Backend/Command.h"
#include "Backend/SwapChain.h"

#include "Renderer/Renderer.h"

namespace wind
{
    RenderGraph::RenderGraph() = default;

    RenderGraph::~RenderGraph()
    {
        for (auto resource : m_resources)
        {
            delete resource;
        }
    }

    void RenderGraph::SetupSwapChain(const Swapchain& swapchain) { m_swapchain = &swapchain; }
    void RenderGraph::SetupFrameData(FrameParms& frameData) { m_currentFrameData = &frameData; }

    void RenderGraph::Exec()
    {
        auto cmdBuffer = m_currentFrameData->cmdBuffer;

        SwapchainStartTrans();

        for (const auto& [name, node] : m_passNodes)
        {
            // insert barrier here
            for (auto depend : node->dependResources)
            {
                if (depend)
                {
                    RenderGraphResource* resource = m_resources[depend.m_index];
                    if (resource->type == RenderGraphResourceType::Texture)
                    {
                        auto texture    = static_cast<RenderGraphTexture*>(resource);
                        auto gpuTexture = texture->GetGPUTexture();
                        gpuTexture->SetImageLayout(cmdBuffer, vk::ImageLayout::eShaderReadOnlyOptimal);
                    }
                }
            }

            for (auto output : node->outputResources)
            {
                if (output)
                {
                    RenderGraphResource* resource = m_resources[output.m_index];
                    if (resource->type == RenderGraphResourceType::Texture)
                    {
                        auto texture    = static_cast<RenderGraphTexture*>(resource);
                        auto gpuTexture = texture->GetGPUTexture();
                        if (utils::IsDepthFormat(gpuTexture->format()))
                        {
                            gpuTexture->SetImageLayout(cmdBuffer, vk::ImageLayout::eDepthAttachmentOptimal);
                        }
                        else
                        {
                            gpuTexture->SetImageLayout(cmdBuffer, vk::ImageLayout::eDepthAttachmentOptimal);
                        }
                    }
                }
            }

            ResourceRegistry registry(*this, node.get());
            node->Execute(registry, cmdBuffer);
        }

        if (m_swapchain)
        {
            SwapchainEndTrans();
            cmdBuffer.end();
            m_swapchain->SubmitCommandBuffer(cmdBuffer,
                                             m_currentFrameData->flightFence,
                                             m_currentFrameData->imageAvailableSemaphore,
                                             m_currentFrameData->renderFinishedSemaphore,
                                             m_currentFrameData->swapchainImageIndex);
        }
    }

    void RenderGraph::MarkAsDirty() { m_dirty = true; }

    RenderGraph::Builder RenderGraph::AddPassInternal(const std::string& name, Scope<RenderGraphPassBase> pass)
    {
        // maybe not good for the result
        if (m_passNodes.contains(name))
        {
            return Builder {*this, m_passNodes[name].get()};
        }
        Scope<PassNode> node   = scope::Create<RenderPassNode>(*this, name, std::move(pass));
        auto            rawPtr = node.get();
        m_passNodes[name]      = std::move(node);
        MarkAsDirty();
        return Builder {*this, rawPtr};
    }

    void RenderGraph::Compile()
    {
        // only compile the graph when graph is change
        if (!m_dirty)
            return;

        for (auto& [name, node] : m_passNodes)
        {
            node->InitResources();
        }

        m_dirty = false;
    }

    vk::RenderingInfo RenderGraph::GetPresentRenderingInfo() const noexcept
    {
        auto index = m_currentFrameData->swapchainImageIndex;
        return m_swapchain->renderingInfo(index);
    }

    void RenderGraph::SwapchainStartTrans()
    {
        command::TransferLayout(m_currentFrameData->cmdBuffer,
                                m_swapchain->image(m_currentFrameData->swapchainImageIndex),
                                1,
                                1,
                                vk::ImageLayout::eUndefined,
                                vk::ImageLayout::eColorAttachmentOptimal);
    }

    void RenderGraph::SwapchainEndTrans()
    {
        command::TransferLayout(m_currentFrameData->cmdBuffer,
                                m_swapchain->image(m_currentFrameData->swapchainImageIndex),
                                1,
                                1,
                                vk::ImageLayout::eColorAttachmentOptimal,
                                vk::ImageLayout::ePresentSrcKHR);
    }

    void RenderGraph::InitGraphResource(RenderGraphHandle handle) { m_resources[handle.m_index]->InitRHI(); }

    void RenderGraph::FreeGrahpResource(RenderGraphHandle handle) { m_resources[handle.m_index]->ReleaseRHI(); }
} // namespace wind