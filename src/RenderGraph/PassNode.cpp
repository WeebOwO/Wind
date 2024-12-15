#include "RenderGraph/PassNode.h"
#include "RenderGraph/ResourceRegistry.h"

namespace wind::rg
{
    RenderPassNode::RenderPassNode(RenderGraph* renderGraph, const std::string& name, RenderGraphPassBase* pass) :
        PassNode(name), m_RenderGraph(renderGraph), m_Pass(pass)
    {}

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

        for (size_t i = 0; i < RenderPassDesc::kAttachmentMax; ++i)
        {
            RenderGraphID<RenderGraphTexture> texture = passDescriptor.attachments.textures[i];
            if (texture)
            {
                
            };
        }

        uint32_t id = m_PassDatas.size();

        return id;
    };
} // namespace wind::rg