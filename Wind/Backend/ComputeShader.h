#pragma once

#include "std.h"

#include "Shader.h"

namespace wind
{

    class ComputeShader : public Shader
    {
    public:
        ComputeShader(GPUDevice& device, const std::string& debugName, const std::vector<uint32_t>& spirvCode);
        
        ~ComputeShader();

        auto GetPipeline() const { return m_pipeline; }

        void BindCommandBuffer(const vk::CommandBuffer& cmdBuffer) const override;

    private:
        static constexpr auto bindPoint = {ShaderTag::ComputeShader};
        vk::ShaderModule      m_computeModule;
        vk::Pipeline          m_pipeline;
        vk::PipelineCache     m_cache;
    };
} // namespace wind