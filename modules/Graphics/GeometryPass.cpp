#include "GeometryPass.h"

#include "Backend/Device.h"

namespace wind
{
    GeometryPass::GeometryPass(Pipeline* pipeline) : m_Pipeline(pipeline) {}

    GeometryPass::~GeometryPass() {}

    void GeometryPass::Draw(vk::CommandBuffer cmdBuffer)
    {
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);
    };
} // namespace wind