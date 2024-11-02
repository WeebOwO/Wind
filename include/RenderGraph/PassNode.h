#pragma once

#include <string>

#include "RenderGraphPass.h"

namespace wind::rg
{
    class RenderGraph;

    class PassNode
    {
    public:
        virtual ~PassNode() = default;
    };

    class RenderPassNode : public PassNode
    {
    public:
        RenderPassNode(RenderGraph* renderGraph, const std::string& name, RenderGraphPassBase* pass);

    private:
        RenderGraph*         m_renderGraph;
        std::string          m_name;
        RenderGraphPassBase* m_pass;
    };
} // namespace wind::rg