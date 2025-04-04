#include "RenderGraphPass.h"

#include "RenderGraph.h"
#include "Core/Log.h"

namespace wind 
{
    void RenderGraphPass::RecordRenderGrpah(RenderGraph& renderGraph)
    {
        // record the render graph
        WIND_CORE_WARN("RenderGraphPass {} record function not implemented", m_PassName);
    }

    void RenderGraphPass::Execute(vk::CommandBuffer cmdBuffer)
    {
        // execute the render graph pass
        WIND_CORE_WARN("RenderGraphPass {} execute function not implemented", m_PassName);
    }
}