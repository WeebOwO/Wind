#pragma once

#include <memory>

#include "Backend/Pipeline.h"
#include "Backend/Shader.h"
#include "RenderPass.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    class Device;

    class GeometryPass : public IRenderPass
    {
    public:
        GeometryPass();
        ~GeometryPass() override;

        void Prepare(Device* m_Device, Shader* vertexShader, Shader* fragmentShader);
        void Draw(vk::CommandBuffer cmdBuffer) override;

        void HandleDirtyShaders(const std::unordered_set<ShaderID>& dirtyShaders,
                                ShaderLibrary*                      shaderLibrary,
                                Device*                             device);

    private:
        std::unique_ptr<Pipeline>    m_Pipeline;
        std::unordered_set<ShaderID> m_ShaderID;
    };
} // namespace wind