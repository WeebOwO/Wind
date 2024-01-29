#include "SceneRenderer.h"

#include "Backend/Texture.h"
#include "Renderer/RenderGraph/PassNode.h"

#include "MeshPass.h"
#include "Renderer.h"
#include "View.h"

#include "RenderGraph/RenderGraphPass.h"
#include "RenderGraph/RenderGraphResource.h"
#include "RenderGraph/RenderGraphTexture.h"
#include "RenderGraph/RenderPassEnum.h"
#include "RenderGraph/ResourceRegistry.h"

#include "Engine/RuntimeContext.h"

#include "Resource/Loader.h"
#include "Resource/Mesh.h"
#include "Resource/VertexFactory.h"

#include "Scene/Scene.h"

namespace wind
{

    void SceneRenderer::Init() {}

    void SceneRenderer::InitView(View& view)
    {
        for (auto meshPassType = MeshPassType::BasePass; meshPassType != MeshPassType::Count;
             meshPassType      = Step(meshPassType))
        {
            BuildMeshDrawCommand(m_renderScene->m_meshPasses[meshPassType]);
        }
    }

    void SceneRenderer::SetViewPort(float offsetX, float offsetY, float width, float height)
    {
        m_viewPortWidth  = uint32_t(width);
        m_viewPortHeight = uint32_t(height);

        m_viewPort.setWidth(width)
                  .setHeight(height)
                  .setX(offsetX)
                  .setY(offsetY)
                  .setMinDepth(0.0)
                  .setMaxDepth(1.0);
    }

    void SceneRenderer::DrawMesh(vk::CommandBuffer commands)
    {
        auto renderer = g_runtimeContext.renderer.get();
        for (const auto& meshDrawCommand : m_cacheMeshDrawCommands[BasePass])
        {
            auto pso = renderer->GetPso(meshDrawCommand.pipelineID);

            commands.setViewport(0, 1, &m_viewPort);

            vk::Rect2D scissor {.offset = {.x = 0, .y = 0},
                                .extent = {.width = (uint32_t)m_viewPort.width, .height = (uint32_t)m_viewPort.height}};
            commands.setScissor(0, 1, &scissor);

            auto vertexBuffer = meshDrawCommand.drawMesh.meshSource->vertexBuffer;
            auto indexBuffer  = meshDrawCommand.drawMesh.meshSource->indexBuffer;

            auto           verBufferHandle = vertexBuffer->buffer();
            vk::DeviceSize offset          = 0;

            commands.bindPipeline(vk::PipelineBindPoint::eGraphics, pso);
            commands.bindVertexBuffers(0, 1, &verBufferHandle, &offset);
            commands.bindIndexBuffer(indexBuffer->buffer(), 0, vk::IndexType::eUint32);

            commands.drawIndexed(3 * meshDrawCommand.drawMesh.meshSource->indices.size(), 1, 0, 0, 0);
        }
    }

    void SceneRenderer::Render(View& view, RenderGraph& rg)
    {
        m_renderScene = g_runtimeContext.activeScene;
        InitView(view);

        auto& blackBoard = rg.GetBlackBoard();

        struct ColorPassData
        {
            AttachmentInfo sceneColor;
        };

        vk::ClearValue clearValue {.color =
                                       vk::ClearColorValue {.float32 = std::array<float, 4> {1.0f, 1.0f, 1.0f, 1.0f}}};

        vk::Rect2D renderArea = {.offset = {.x = 0, .y = 0},
                                 .extent = {.width = m_viewPortWidth, .height = m_viewPortHeight}};

        auto& colorPass = rg.AddPass<ColorPassData>(
            "LightingPass",
            [&](RenderGraph::Builder& builder, ColorPassData& data) {
                // set the data
                RenderGraphTexture::Desc desc {.width  = m_viewPortWidth,
                                               .height = m_viewPortHeight,
                                               .depth  = 1,
                                               .type   = TextureViewType::Texture2D,
                                               .format = vk::Format::eR8G8B8A8Srgb,
                                               .layout = vk::ImageLayout::eColorAttachmentOptimal,
                                               .usage  = vk::ImageUsageFlagBits::eColorAttachment |
                                                        vk::ImageUsageFlagBits::eSampled};

                data.sceneColor = AttachmentInfo {.texture    = builder.CreateTexture("SceneColor", desc),
                                                  .loadop     = vk::AttachmentLoadOp::eLoad,
                                                  .storeop    = vk::AttachmentStoreOp::eStore,
                                                  .clearValue = clearValue};

                RenderPassNode::RenderDesc renderDesc {
                    .attchments = {.color = {data.sceneColor}, .depth = {}, .stencil = {}}, .renderArea = renderArea};

                builder.DeclareRenderPass(renderDesc);
            },
            [&](ResourceRegistry& resourceRegistry, ColorPassData& data, vk::CommandBuffer cb) {
                cb.beginRendering(resourceRegistry.GetRenderingInfo());
                DrawMesh(cb);
                cb.endRendering();
            },
            PassType::Graphics);

        blackBoard["output"] = colorPass->sceneColor.texture;
    }

    void SceneRenderer::BuildMeshDrawCommand(const MeshPass& meshPass)
    {
        auto                renderer = g_runtimeContext.renderer.get();
        RenderGraphPassType graphPassType =
            meshPass.type == MeshPassType::BasePass ? RenderGraphPassType::MeshPassMRT : RenderGraphPassType::MeshPass;
        m_cacheMeshDrawCommands[meshPass.type].clear();

        for (auto meshProxy : meshPass.staticMeshes)
        {
            MeshDrawCommand meshDrawCommand;
            meshDrawCommand.drawMesh.firstVertex = 0;
            meshDrawCommand.drawMesh.meshSource  = &meshProxy->meshSource;
            meshDrawCommand.drawMesh.vertexCount = meshProxy->meshSource.vertices.size();
            meshDrawCommand.drawMesh.indexCount  = meshProxy->meshSource.indices.size();
            meshDrawCommand.materialProxy        = meshProxy->material;

            meshDrawCommand.pipelineID =
                renderer->CachePso(*meshProxy->material, VertexFactoryType::StaicMesh, graphPassType);

            m_cacheMeshDrawCommands[meshPass.type].push_back(meshDrawCommand);
        }
    }

} // namespace wind