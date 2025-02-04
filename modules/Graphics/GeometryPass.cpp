#include "GeometryPass.h"

#include "Backend/Device.h"

namespace wind
{
    GeometryPass::GeometryPass()
    {
        m_ShaderID.insert(ShaderID::Triangle_VS);
        m_ShaderID.insert(ShaderID::Triangle_PS);
    }

    void GeometryPass::Prepare(Device* device, ShaderLibrary* library)
    {
        GraphicPipelineDesc desc;

        vk::Device vkDevice = device->GetDevice();

        vk::PipelineLayout pipelineLayout = vkDevice.createPipelineLayout({});

        Shader* vertexShader   = library->GetShader(ShaderID::Triangle_VS);
        Shader* fragmentShader = library->GetShader(ShaderID::Triangle_PS);

        desc.SetShaders(vertexShader->GetBlobData().module, fragmentShader->GetBlobData().module)
            .SetLayout(pipelineLayout)
            .SetInputTopology(vk::PrimitiveTopology::eTriangleList)
            .SetPolygonMode(vk::PolygonMode::eFill)
            .SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise)
            .SetMultisamplingNone()
            .EnableDepthTest(true, vk::CompareOp::eLessOrEqual);

        m_Pipeline = device->CreateResourceUnique<Pipeline>(desc);
        m_Pipeline->InitRHI();
    }

    GeometryPass::~GeometryPass()
    {
        m_Pipeline->ReleaseRHI();
        m_Pipeline.reset();
    }

    void GeometryPass::Draw(vk::CommandBuffer cmdBuffer)
    {
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);
    };

    void GeometryPass::HandleDirtyShaders(const std::unordered_set<ShaderID>& dirtyShaders,
                                          ShaderLibrary*                      shaderLibrary,
                                          Device*                             device)
    {
        for (auto& id : dirtyShaders)
        {
            if (m_ShaderID.contains(id))
            {
                m_Pipeline->ReleaseRHI();
            }
        }

        Prepare(device, shaderLibrary);
    }
} // namespace wind