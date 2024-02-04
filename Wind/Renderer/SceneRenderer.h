#pragma once

#include "std.h"

#include "MeshPass.h"
#include "RenderGraph/RenderGraph.h"

#include "Backend/Command.h"

namespace wind
{
    class Scene;
    class View;

    class SceneRenderer
    {
    public:
        SceneRenderer()  = default;
        ~SceneRenderer() = default;

        void Init();
        void SetViewPort(float offsetX, float offsetY, float width, float height);

        void Render(View& view, RenderGraph& renderGraph, const Scene& scene);

    private:
        void InitView(View& view, const Scene& scene); // Dispatch MeshPass

        void BuildMeshDrawCommand(const MeshPass& meshPass);
        void DrawMesh(vk::CommandBuffer commands);

        vk::Viewport m_viewPort {};

        PerpassData<std::vector<MeshDrawCommand>> m_cacheMeshDrawCommands;

        uint32_t m_viewPortWidth  = 0;
        uint32_t m_viewPortHeight = 0;
    };
} // namespace wind