#include "RenderGraph.h"

#include "PassNode.h"
#include "ResourceRegistry.h"

#include "Core/Log.h"

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
        auto renderEncoder = m_currentFrameData->renderEncoder;
        renderEncoder->Begin();
        SwapchainStartTrans();

        for (const auto& [name, node] : m_passNodes)
        {
            ResourceRegistry registry(*this, node.get());

            // insert barrier here
            for (auto depend : node->dependResources)
            {   
                if (depend)
                {
                    auto resource = m_resources[depend];
                    if(resource->type == RenderGraphResourceType::Texture) {
                        RenderGraphTexture* texture = static_cast<RenderGraphTexture*>(resource);
                        
                    }
                }
            }

            for (auto output : node->outputResources)
            {
                
            }

            node->Execute(registry, *renderEncoder);
        }

        if (m_swapchain)
        {
            SwapchainEndTrans();
            m_swapchain->SubmitCommandBuffer(renderEncoder->Finish(),
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
        return m_swapchain->GetRenderingInfo(index);
    }

    void RenderGraph::SwapchainStartTrans()
    {
        m_currentFrameData->renderEncoder->TransferImageLayout(
            m_swapchain->GetImage(m_currentFrameData->swapchainImageIndex),
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::ImageSubresourceRange {.aspectMask     = vk::ImageAspectFlagBits::eColor,
                                       .baseMipLevel   = 0,
                                       .levelCount     = 1,
                                       .baseArrayLayer = 0,
                                       .layerCount     = 1});
    }

    void RenderGraph::SwapchainEndTrans()
    {
        m_currentFrameData->renderEncoder->TransferImageLayout(
            m_swapchain->GetImage(m_currentFrameData->swapchainImageIndex),
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::AccessFlagBits::eNone,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::ImageSubresourceRange {.aspectMask     = vk::ImageAspectFlagBits::eColor,
                                       .baseMipLevel   = 0,
                                       .levelCount     = 1,
                                       .baseArrayLayer = 0,
                                       .layerCount     = 1});
    }

    void RenderGraph::InitGraphResource(RenderGraphHandle handle) { m_resources[handle.m_index]->InitRHI(); }

    void RenderGraph::FreeGrahpResource(RenderGraphHandle handle) { m_resources[handle.m_index]->ReleaseRHI(); }
} // namespace wind