#include "RenderGraph/PassNode.h"

#include "RenderGraph/RenderGraph.h"
#include "RenderGraph/RenderGraphResource.h"
#include "RenderGraph/RenderGraphTexture.h"
#include "RenderGraph/ResourceRegistry.h"
#include "vulkan/vulkan_structs.hpp"

namespace wind::rg
{
    RenderPassNode::RenderPassNode(RenderGraph* renderGraph, const std::string& name, RenderGraphPassBase* pass) :
        PassNode(name), m_RenderGraph(renderGraph), m_Pass(pass)
    {}

    void RenderPassNode::Prepare(ResourceRegistry& registry)
    {
        // prepare the pass
        rg::RenderGraphAllocator* allocator = m_RenderGraph->GetAllocator();
        for (auto& resource : constructResource)
        {
            resource->Create(allocator);
        }

        auto& passData = m_PassDatas[0];

        // create rendering info
        vk::ClearColorValue        defaultClearColor(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f});
        vk::ClearDepthStencilValue defaultClearDepthStencil(1.0f, 0);

        for (size_t i = 0; i < RenderPassDesc::kAttachmentMax; ++i)
        {
            RenderGraphID<RenderGraphTexture> texture = passData.desc.attachments.textures[i];
            if (texture)
            {
                auto&          resource = registry.Get(texture);
                vk::ClearValue clearValue {};

                vk::RenderingAttachmentInfo renderAttachmentInfo;
                renderAttachmentInfo.imageLayout = resource.desc.initialLayout;
                renderAttachmentInfo.clearValue  = clearValue;
                renderAttachmentInfo.imageView   = resource.view;
                renderAttachmentInfo.loadOp      = resource.desc.loadOp;
                renderAttachmentInfo.storeOp     = resource.desc.storeOp;

                if (resource.desc.depth)
                {
                    clearValue.depthStencil = defaultClearDepthStencil;
                    m_DepthAttachmentInfo  = renderAttachmentInfo;
                }
                else
                {
                    clearValue.color = defaultClearColor;
                    m_RenderAttachmentInfos.push_back(renderAttachmentInfo);
                }
            }
        };

        auto viewPort = passData.desc.viewPort;

        vk::Rect2D renderArea 
        {
            vk::Offset2D {0, 0},
            vk::Extent2D {(uint32_t)viewPort.width, (uint32_t)viewPort.height}
        };

        m_RenderingInfo.setColorAttachments(m_RenderAttachmentInfos)
                       .setPDepthAttachment(&m_DepthAttachmentInfo)
                       .setRenderArea(renderArea)
                       .setLayerCount(1);
    }

    void RenderPassNode::Execute(ResourceRegistry& registry, CommandStream& commandStream)
    {
        m_Pass->Execute(registry, commandStream);
    }

    uint32_t RenderPassNode::DeclareRenderPass(const std::string&                name,
                                               const RenderPassDesc::Descriptor& passDescriptor)
    {
        PassData passData;
        passData.desc = passDescriptor;
        passData.name = name;

        // create rendering info
        vk::ClearColorValue        defaultClearColor(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f});
        vk::ClearDepthStencilValue defaultClearDepthStencil(1.0f, 0);

        for (size_t i = 0; i < RenderPassDesc::kAttachmentMax; ++i)
        {
            RenderGraphID<RenderGraphTexture> texture = passDescriptor.attachments.textures[i];
            // todo: check if the texture is valid
        }

        // we don't support subpass, so this should always be 0
        uint32_t id = m_PassDatas.size();
        m_PassDatas.push_back(passData);

        return id;
    };
} // namespace wind::rg