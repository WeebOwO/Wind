#include "RenderGraphPass.h"

#include "Core/Log.h"
#include "RenderGraph.h"

namespace wind
{
    void PassNode::RecordRenderGrpah(RenderGraph& renderGraph)
    {
        // record the render graph
        WIND_CORE_WARN("RenderGraphPass {} record function not implemented", m_PassName);
    }

    void PassNode::Execute(vk::CommandBuffer cmdBuffer)
    {
        // execute the render graph pass
        WIND_CORE_WARN("RenderGraphPass {} execute function not implemented", m_PassName);
    }

    void RenderPassNode::BeforeRendering(vk::CommandBuffer cmdBuffer)
    {
        RenderGraph& renderGraph = *m_RenderGraph;

        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        vk::RenderingAttachmentInfo              depthAttachment;
        vk::RenderingAttachmentInfo              stencilAttachment;

        bool hasDepth   = false;
        bool hasStencil = false;

        // loop through the attachments and set the color attachments
        for (int i = 0; i < MAX_RENDER_TARGET_COUNT - 2; ++i)
        {
            if (m_Descriptor.attachments.color[i].isInitialized())
            {
                VirtualResource* resource = renderGraph.GetResource(m_Descriptor.attachments.color[i]);
                if (resource != nullptr)
                {
                    colorAttachments.push_back(vk::RenderingAttachmentInfo {
                        .imageView   = resource->As<VirtualImage>()->imageView,
                        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                        .loadOp      = vk::AttachmentLoadOp::eClear,
                        .storeOp     = vk::AttachmentStoreOp::eStore,
                        .clearValue  = m_Descriptor.clearValue,
                    });
                }
            }
        }

        // set the depth attachment
        if (m_Descriptor.attachments.depth.isInitialized())
        {
            VirtualResource* resource = renderGraph.GetResource(m_Descriptor.attachments.depth);
            if (resource != nullptr)
            {
                depthAttachment = vk::RenderingAttachmentInfo {
                    .imageView   = resource->As<VirtualImage>()->imageView,
                    .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    .loadOp      = vk::AttachmentLoadOp::eClear,
                    .storeOp     = vk::AttachmentStoreOp::eStore,
                };
                hasDepth = true;
            }
        }

        // set the stencil attachment
        if (m_Descriptor.attachments.stencil.isInitialized())
        {
            VirtualResource* resource = renderGraph.GetResource(m_Descriptor.attachments.stencil);
            if (resource != nullptr)
            {
                stencilAttachment = vk::RenderingAttachmentInfo {
                    .imageView   = resource->As<VirtualImage>()->imageView,
                    .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    .loadOp      = vk::AttachmentLoadOp::eClear,
                    .storeOp     = vk::AttachmentStoreOp::eStore,
                };
                hasStencil = true;
            }
        }

        // set the rendering info
        vk::RenderingInfo renderingInfo = vk::RenderingInfo {
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
        vk::Viewport viewPort 
        {
            .width = static_cast<float>(m_Descriptor.viewPort.width),
            .height = static_cast<float>(m_Descriptor.viewPort.height),
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

    void RenderPassNode::AfterRendering(vk::CommandBuffer cmdBuffer)
    {
        // end the rendering
        cmdBuffer.endRendering();
    }
} // namespace wind