#pragma once

#include "Shader.h"

namespace wind
{
    class ShaderMap;

    class RasterShader final : public Shader
    {
    public:
        friend class GraphicsPipelineBuilder;

        RasterShader(GPUDevice&              device,
                     const std::string&      name,
                     const vk::ShaderModule& vertexModule,
                     const vk::ShaderModule& fragModule) noexcept;

        RasterShader(GPUDevice&         device,
                     const std::string& name,
                     const std::string& vertexName,
                     const std::string& fragName);

        ~RasterShader();

        vk::PipelineLayout GetPipelineLayout() const noexcept { return m_layout; }

        auto GetVertexModule() const noexcept { return m_vertexModule; }
        auto GetFragModule() const noexcept { return m_fragModule; }

    private:
        vk::ShaderModule m_vertexModule;
        vk::ShaderModule m_fragModule;
    };

    using RasterShaderRef = Ref<RasterShader>;
} // namespace wind