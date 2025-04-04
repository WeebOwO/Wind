#include "GeometryPass.h"

#include "RenderGraph/RenderGraph.h"

namespace wind
{
    GeometryPass::GeometryPass(PipelineID id, PSOCache* psoCache) :
        m_PsoCacheLibrary(psoCache), m_PipelineID(id), RenderGraphPass("GBufferPass")
    {}

    GeometryPass::~GeometryPass() {}

    void GeometryPass::RecordRenderGrpah(RenderGraph& renderGraph)
    {
        // setup the pass
        auto builder = renderGraph.AddPass<GeometryPass>(m_PipelineID, m_PsoCacheLibrary);
    }

    void GeometryPass::Execute(vk::CommandBuffer cmdBuffer)
    {
        Pipeline* pipeline = m_PsoCacheLibrary->GetPipeline(m_PipelineID);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);
    };
} // namespace wind