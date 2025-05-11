#pragma once

#include <string>

#include "RenderGraphHandle.h"
#include "RenderGraphNode.h"
#include "VirtualResource.h"

namespace wind
{
    class RenderGraph;
    struct ResourceNode : public Node
    {
        ResourceNode(RenderGraph& graph, const std::string& name, RenderGraphHandle handle) :
            renderGraph(graph), name(name), producePass(nullptr), handle(handle), resource(nullptr)
        {
            nodeType = NodeType::Resource;
        };

        RenderGraph&           renderGraph;
        std::string            name;
        PassNode*              producePass;
        std::vector<PassNode*> usePasses;
        RenderGraphHandle      handle;
        VirtualResource*       resource;
    };
} // namespace wind