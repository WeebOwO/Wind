#include "GeometryPass.h"

#include "Graphics/GlobalRT.h"
#include "Graphics/View.h"
#include "RenderGraph/RenderGraph.h"
#include "RenderGraph/RenderGraphBuilder.h"

#include "Core/Log.h"
#include "ShaderCompiler/SlangCompiler.h"

namespace wind
{
    GeometryPass::GeometryPass(PipelineManager* pipelineManager) :
        m_PipelineManager(pipelineManager), RenderPassNode("GBufferPass"), m_CustomPipelineID(-1)
    {
        BakeRenderPipeline();
    }

    GeometryPass::~GeometryPass() {}

    void GeometryPass::Setup(RenderGraphBuilder& builder)
    {
        // setup the pass
        m_SceneColorHandle        = builder.GetResourceHandle(GlobalRT::BackBuffer);
        vk::ClearValue clearValue = {.color = vk::ClearColorValue(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f})};

        std::vector<AttachmentInfo> renderTargets = {
            {m_SceneColorHandle, clearValue, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore},
        };

        SetRenderTargets(renderTargets);
    }

    void GeometryPass::Execute(RenderGraphUpdateContext& context)
    {
        vk::CommandBuffer cmdBuffer = context.commandStream->GetCommandBuffer();

        if (m_CustomPipelineID == -1)
        {
            // log error
            WIND_CORE_ERROR("GeometryPass: Custom pipeline not baked");
            return;
        }

        Pipeline* pipeline = m_PipelineManager->GetPipeline(m_CustomPipelineID);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());

        auto& scene = context.scene;

        for (const auto& mesh : scene->GetMeshes())
        {
            for (const auto& subMesh : mesh.subMeshes)
            {
                // clang-format off
                DrawCommand drawCommand = 
                {
                    .vertexBuffer = subMesh.vertexBuffer,
                    .indexBuffer  = subMesh.indexBuffer,
                    .indexCount   = static_cast<uint32_t>(subMesh.indices.size()),
                    .instanceCount = 1,
                };

                context.commandStream->Draw(drawCommand);
                // clang-format on
            }
        }
    };

    void GeometryPass::BakeRenderPipeline()
    {
        const std::string shaderPath    = "shaders/Triangle.slang";
        const std::string vertexEntry   = "vertexMain";
        const std::string fragmentEntry = "fragmentMain";

        std::vector<uint32_t> vertexSpirv, fragmentSpirv;
        bool                  success =
            SlangCompiler::CompileFromFile(shaderPath, vertexEntry, fragmentEntry, vertexSpirv, fragmentSpirv);

        if (!success)
        {
            WIND_CORE_ERROR("Failed to compile shader: {0}", shaderPath);
            return;
        }

        WIND_CORE_INFO("Successfully compiled shader pipeline: {0}", shaderPath);
        WIND_CORE_INFO("Vertex SPIR-V size: {0} words", vertexSpirv.size());
        WIND_CORE_INFO("Fragment SPIR-V size: {0} words", fragmentSpirv.size());

        BlobData blob;
        blob.stage        = vk::ShaderStageFlagBits::eVertex;
        blob.spirv        = vertexSpirv;
        auto vertexModule = m_PipelineManager->GetShaderLibrary()->CreateShader(blob);

        blob.stage          = vk::ShaderStageFlagBits::eFragment;
        blob.spirv          = fragmentSpirv;
        auto fragmentModule = m_PipelineManager->GetShaderLibrary()->CreateShader(blob);

        GraphicPipelineDesc desc;

        desc.SetShaders(vertexModule->GetNativeHandle(), fragmentModule->GetNativeHandle());
        desc.SetInputTopology(vk::PrimitiveTopology::eTriangleList);
        desc.SetPolygonMode(vk::PolygonMode::eFill);
        desc.SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);
        desc.SetMultisamplingNone();
        desc.SetBlendMode(BlendMode::Opaque);
        desc.SetLayout(m_PipelineManager->GetEmptyPipelineLayout());
        desc.SetVertexInputType(VertexInputType::StaticMesh);

        m_CustomPipelineID = m_PipelineManager->CreatePipeline(desc);
    }
} // namespace wind