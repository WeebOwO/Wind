#pragma once

#include <memory>

#include "Backend/Pipeline.h"
#include "RenderGraph/RenderGraphPass.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    class Device;

    class GeometryPass : public RenderGraphPass
    {
    public:
        GeometryPass(Pipeline* pipeline);
        ~GeometryPass() override;

        void Draw(vk::CommandBuffer cmdBuffer) override;

    private:
        Pipeline*                    m_Pipeline;
        std::unordered_set<ShaderID> m_ShaderID;
        vk::RenderingInfo            m_RenderingInfo;
    };
} // namespace wind