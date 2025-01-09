#pragma once

#include <string>

#include "PassDesc.h"
#include "RenderGraphPass.h"

namespace wind::rg
{
    class RenderGraph;
    class ResourceRegistry;
    class ResourceNode;

    class PassNode
    {
    public:
        PassNode(const std::string& name) noexcept : name(name) {}
        virtual ~PassNode() = default;

        virtual void Prepare(ResourceRegistry& registry) {}
        virtual void Execute(ResourceRegistry& registry, CommandStream& commandStream) = 0;

        std::string GetPassName() const { return name; }

        std::string                    name;
        wind::vector<VirutalResource*> constructResource;
        wind::vector<VirutalResource*> destroyResource;
    };

    class RenderPassNode : public PassNode
    {
    public:
        struct PassData
        {
            std::string                name;
            RenderPassDesc::Descriptor desc;
            std::vector<ResourceNode*> inComingResources;
            std::vector<ResourceNode*> outGoingResources;
        };

        RenderPassNode(RenderGraph* renderGraph, const std::string& name, RenderGraphPassBase* pass);

        void Prepare(ResourceRegistry& registry) override;
        void Execute(ResourceRegistry& registry, CommandStream& commandStream) override;

        uint32_t DeclareRenderPass(const std::string& name, const RenderPassDesc::Descriptor& passData);

        auto GetRenderingInfo() const { return m_RenderingInfo; }

    private:
        RenderGraph*                             m_RenderGraph;
        RenderGraphPassBase*                     m_Pass;
        std::vector<PassData>                    m_PassDatas;
        std::vector<vk::RenderingAttachmentInfo> m_RenderAttachmentInfos {};
        vk::RenderingAttachmentInfo              m_DepthAttachmentInfo {};
        vk::RenderingInfo                        m_RenderingInfo {};
    };
} // namespace wind::rg