#pragma once

#include "Graphics/PsoCache.h"

#include "RenderGraphNode.h"

namespace wind
{
    class RenderGraphUpdateContext;
    class RenderGraph;

    class RenderGraphPass : public PassNode
    {
    public:
        RenderGraphPass(const std::string& name) : 
            m_PassName(name) {};
            
        virtual void RecordRenderGrpah(RenderGraph& renderGraph);
        virtual void Execute(vk::CommandBuffer cmdBuffer);

        virtual ~RenderGraphPass() = default;

    protected:
        std::string m_PassName;
    };

} // namespace wind