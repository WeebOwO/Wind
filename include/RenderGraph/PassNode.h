#pragma once

#include "RenderGraph/RenderGraphResource.h"
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
        PassNode(const std::string& name) noexcept : m_Name(name) {}
        virtual ~PassNode() = default;

        virtual void Execute(ResourceRegistry& registry, CommandStream& commandStream) = 0;

        std::string GetPassName() const { return m_Name; }

    protected:
        std::string m_Name;
        wind::vector<VirutalResource*> m_Construct;
        wind::vector<VirutalResource*> m_Destroy;
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

        auto GetRenderingInfo() const { return m_RenderingInfo; }

    private:
        RenderGraph*          m_RenderGraph;
        RenderGraphPassBase*  m_Pass;
        std::vector<PassData> m_PassDatas;
        vk::RenderingInfo     m_RenderingInfo;
    };
} // namespace wind::rg