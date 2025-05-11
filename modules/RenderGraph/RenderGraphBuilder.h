#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphHandle.h"
#include "VirtualResource.h"

namespace wind
{
    class RenderGraph;

    class RenderGraphBuilder
    {
    public:
        RenderGraphBuilder(RenderGraph& renderGraph, PassNode* currentPass);

        RenderGraphHandle Create(const RDGResourceDesc& resourceDesc);
        RenderGraphHandle Import(VirtualResource* resource, const std::string& name);
        RenderGraphHandle GetResourceHandle(const std::string& name);
        RenderGraphHandle Write(RenderGraphHandle handle);

    private:
        RenderGraph& m_RenderGraph;
        PassNode*    m_Pass;
    };
} // namespace wind