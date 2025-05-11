#include "RenderGraphPass.h"

#include "Core/Log.h"
#include "RenderGraph.h"
#include "RenderGraphBuilder.h"

namespace wind
{
    void PassNode::Setup(RenderGraphBuilder& renderGraph)
    {
        // record the render graph
        WIND_CORE_WARN("RenderGraphPass {} setup function not implemented", m_PassName);
    }

    void PassNode::Execute(vk::CommandBuffer cmdBuffer)
    {
        // execute the render graph pass
        WIND_CORE_WARN("RenderGraphPass {} execute function not implemented", m_PassName);
    }

    void RenderPassNode::BeginRendering(vk::CommandBuffer cmdBuffer)
    {
        RenderGraph& renderGraph = *m_RenderGraph;

        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        vk::RenderingAttachmentInfo              depthAttachment;
        vk::RenderingAttachmentInfo              stencilAttachment;

        bool hasDepth   = false;
        bool hasStencil = false;

        // loop through the attachments and set the color attachments
        for (auto colorAttachment : m_Descriptor.renderTargets)
        {
            if (colorAttachment.handle.isInitialized())
            {
                VirtualResource* resource = renderGraph.GetResource(colorAttachment.handle);
                if (resource != nullptr)
                {
                    colorAttachments.push_back(vk::RenderingAttachmentInfo {
                        .imageView   = resource->As<VirtualImage>()->imageView,
                        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                        .loadOp      = colorAttachment.loadOp,
                        .storeOp     = colorAttachment.storeOp,
                        .clearValue  = colorAttachment.clearValue,
                    });
                }
            }
        }

        // set the depth attachment
        if (m_Descriptor.depth.handle.isInitialized())
        {
            VirtualResource* resource = renderGraph.GetResource(m_Descriptor.depth.handle);
            if (resource != nullptr)
            {
                depthAttachment = vk::RenderingAttachmentInfo {
                    .imageView   = resource->As<VirtualImage>()->imageView,
                    .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    .loadOp      = m_Descriptor.depth.loadOp,
                    .storeOp     = m_Descriptor.depth.storeOp,
                    .clearValue  = m_Descriptor.depth.clearValue,
                };
                hasDepth = true;
            }
        }

        // set the stencil attachment
        if (m_Descriptor.stencil.handle.isInitialized())
        {
            VirtualResource* resource = renderGraph.GetResource(m_Descriptor.stencil.handle);
            if (resource != nullptr)
            {
                stencilAttachment = vk::RenderingAttachmentInfo {
                    .imageView   = resource->As<VirtualImage>()->imageView,
                    .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    .loadOp      = m_Descriptor.stencil.loadOp,
                    .storeOp     = m_Descriptor.stencil.storeOp,
                    .clearValue  = m_Descriptor.stencil.clearValue,
                };
                hasStencil = true;
            }
        }

        // set the rendering info
        vk::RenderingInfo renderingInfo = vk::RenderingInfo 
        {
            .renderArea           = m_Descriptor.renderArea,
            .layerCount           = 1,
            .colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size()),
            .pColorAttachments    = colorAttachments.data(),
            .pDepthAttachment     = hasDepth ? &depthAttachment : nullptr,
            .pStencilAttachment   = hasStencil ? &stencilAttachment : nullptr,
        };

        // begin the rendering
        cmdBuffer.beginRendering(renderingInfo);

        // set the viewport and scissor
        vk::Viewport viewPort {
            .width    = static_cast<float>(m_Descriptor.viewPort.width),
            .height   = static_cast<float>(m_Descriptor.viewPort.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        cmdBuffer.setViewport(0, 1, &viewPort);

        vk::Rect2D scissor {
            .offset = {0, 0},
            .extent = {m_Descriptor.viewPort.width, m_Descriptor.viewPort.height},
        };

        cmdBuffer.setScissor(0, 1, &scissor);
    }

    void RenderPassNode::EndRendering(vk::CommandBuffer cmdBuffer)
    {
        // end the rendering
        cmdBuffer.endRendering();
    }
} // namespace wind