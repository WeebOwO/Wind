#include "GeometryPass.h"

namespace wind
{
    GeometryPass::GeometryPass(PipelineID id, PSOCache* psoCache) : 
    m_PipelineID(id), RenderGraphPass(psoCache) {}

    GeometryPass::~GeometryPass() {}

    void GeometryPass::Setup(RenderGraphBuilder& builder)
    {
        // setup the pass
    }

    void GeometryPass::Draw(vk::CommandBuffer cmdBuffer)
    {
        Pipeline* pipeline = m_PsoCacheLibrary->GetPipeline(m_PipelineID);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);
    };
} // namespace wind