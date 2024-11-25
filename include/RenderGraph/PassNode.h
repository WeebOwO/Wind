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
        PassNode(const std::string& name) noexcept : m_name(name) {}
        virtual ~PassNode() = default;

        virtual void Execute(ResourceRegistry& registry, CommandStream& commandStream) = 0;

        std::string GetPassName() const { return m_name; }

    private:
        std::string m_name;
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

        void Execute(ResourceRegistry& registry, CommandStream& commandStream) override;

        uint32_t DeclareRenderPass(const std::string& name, const RenderPassDesc::Descriptor& passData);

    private:
        RenderGraph*          m_RenderGraph;
        RenderGraphPassBase*  m_Pass;
        std::vector<PassData> m_PassDatas;
    };
} // namespace wind::rg