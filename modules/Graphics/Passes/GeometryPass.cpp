#include "GeometryPass.h"

#include "Graphics/GlobalRT.h"
#include "Graphics/View.h"
#include "RenderGraph/RenderGraph.h"
#include "RenderGraph/RenderGraphBuilder.h"
#include "RenderGraph/RenderGraphResourceRegistry.h"


namespace wind
{
    GeometryPass::GeometryPass(PipelineID id, PSOCache* psoCache) :
        m_PsoCacheLibrary(psoCache), m_PipelineID(id), RenderPassNode("GBufferPass")
    {}

    GeometryPass::~GeometryPass() {}

    void GeometryPass::Setup(RenderGraphBuilder& builder)
    {
        // setup the pass
        m_SceneColorHandle        = builder.GetResourceHandle(GlobalRT::BackBuffer);
        vk::ClearValue clearValue = {.color = vk::ClearColorValue(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f})};

        std::vector<AttachmentInfo> renderTargets = {
            {m_SceneColorHandle, clearValue, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore},
        };

        SetRenderTargets(renderTargets);

        m_Descriptor.viewPort   = m_View->viewport;
        m_Descriptor.clearValue = clearValue;
        m_Descriptor.renderArea = vk::Rect2D {{0, 0}, m_View->viewport.width, m_View->viewport.height};
    }

    void GeometryPass::Execute(RenderGraphUpdateContext& context)
    {
        vk::CommandBuffer cmdBuffer = context.cmdBuffer;

        BeginRendering(cmdBuffer);

        Pipeline* pipeline = m_PsoCacheLibrary->GetPipeline(m_PipelineID);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);

        EndRendering(cmdBuffer);
    };

    void GeometryPass::BlitToBackBuffer(vk::CommandBuffer cmdBuffer, vk::Image color, vk::Image backBuffer)
    {
        // insert a barrier to transition the color image to transfer source layout
        vk::ImageMemoryBarrier barrier;
        barrier.image                           = color;
        barrier.srcAccessMask                   = vk::AccessFlagBits::eColorAttachmentWrite;
        barrier.dstAccessMask                   = vk::AccessFlagBits::eTransferRead;
        barrier.oldLayout                       = vk::ImageLayout::eColorAttachmentOptimal;
        barrier.newLayout                       = vk::ImageLayout::eTransferSrcOptimal;
        barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;

        cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                  vk::PipelineStageFlagBits::eTransfer,
                                  vk::DependencyFlags(),
                                  0,
                                  nullptr,
                                  0,
                                  nullptr,
                                  1,
                                  &barrier);

        // Blit the scene color to the back buffer
        vk::ImageBlit blitRegion;
        blitRegion.srcOffsets[0] = {0, 0, 0};
        blitRegion.srcOffsets[1] = {
            static_cast<int32_t>(m_View->viewport.width), static_cast<int32_t>(m_View->viewport.height), 1};
        blitRegion.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        blitRegion.srcSubresource.layerCount     = 1;
        blitRegion.srcSubresource.mipLevel       = 0;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.dstOffsets[0]                 = {0, 0, 0};
        blitRegion.dstOffsets[1]                 = {
            static_cast<int32_t>(m_View->viewport.width), static_cast<int32_t>(m_View->viewport.height), 1};
        blitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstSubresource.mipLevel   = 0;

        cmdBuffer.blitImage(color,
                            vk::ImageLayout::eTransferSrcOptimal,
                            backBuffer,
                            vk::ImageLayout::eTransferDstOptimal,
                            blitRegion,
                            vk::Filter::eLinear);

        // insert a barrier to transition the back buffer color attachment layout
        barrier.image                           = backBuffer;
        barrier.srcAccessMask                   = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask                   = vk::AccessFlagBits::eShaderRead;
        barrier.oldLayout                       = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout                       = vk::ImageLayout::eColorAttachmentOptimal;
        barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;

        cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                  vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                  vk::DependencyFlags(),
                                  0,
                                  nullptr,
                                  0,
                                  nullptr,
                                  1,
                                  &barrier);
    }
} // namespace wind