#include "PassNode.h"

#include "RenderGraph.h"
#include "ResourceNode.h"
#include "ResourceRegistry.h"

namespace wind
{
    RenderPassNode::RenderPassNode(RenderGraph& rg, const std::string& name, Scope<RenderGraphPassBase> pass) :
        PassNode(rg), m_debugName(name), m_passBase(std::move(pass))
    {}

    void RenderPassNode::DeclareRenderTarget(const RenderDesc& desc)
    {
        m_renderDesc = desc;

        for (auto attachment : desc.attchments.array)
        {
            if (attachment.texture)
            {
                outputResources.push_back(attachment.texture);
            }
        }
    }

    void RenderPassNode::InitResources()
    {
        auto desc = m_renderDesc;

        // create the renderinginfo for vulkan
        for (auto attachment : desc.attchments.color)
        {
            if (attachment.texture)
            {
                renderGraph.InitGraphResource(attachment.texture);
                auto texture = renderGraph.Get(attachment.texture);

                vk::RenderingAttachmentInfo attachmentInfo {.imageView   = texture->GetImageView(),
                                                            .imageLayout = texture->GetLayout(),
                                                            .loadOp      = attachment.loadop,
                                                            .storeOp     = attachment.storeop,
                                                            .clearValue  = attachment.clearValue};
                m_colorAttachmentInfos.push_back(attachmentInfo);
            }
        }

        if (!m_colorAttachmentInfos.empty())
        {
            m_renderingInfo.setColorAttachments(m_colorAttachmentInfos)
                .setColorAttachmentCount(m_colorAttachmentInfos.size());
        }

        if (desc.attchments.depth.texture)
        {
            renderGraph.InitGraphResource(desc.attchments.depth.texture);
            auto texture = renderGraph.Get(desc.attchments.depth.texture);

            m_depthAttachmentInfo = vk::RenderingAttachmentInfo {.imageView   = texture->GetImageView(),
                                                                 .imageLayout = texture->GetLayout(),
                                                                 .loadOp      = desc.attchments.depth.loadop,
                                                                 .storeOp     = desc.attchments.depth.storeop,
                                                                 .clearValue  = desc.attchments.depth.clearValue};
            m_renderingInfo.setPDepthAttachment(&m_depthAttachmentInfo);
        }

        if (desc.attchments.stencil.texture)
        {
            renderGraph.InitGraphResource(desc.attchments.stencil.texture);
            auto texture = renderGraph.Get(desc.attchments.depth.texture);

            m_stencilAttachmentInfo = vk::RenderingAttachmentInfo {.imageView   = texture->GetImageView(),
                                                                   .imageLayout = texture->GetLayout(),
                                                                   .loadOp      = desc.attchments.depth.loadop,
                                                                   .storeOp     = desc.attchments.depth.storeop,
                                                                   .clearValue  = desc.attchments.depth.clearValue};
            m_renderingInfo.setPStencilAttachment(&m_stencilAttachmentInfo);
        }

        m_renderingInfo.setLayerCount(1).setRenderArea(desc.renderArea);
    }
} // namespace wind