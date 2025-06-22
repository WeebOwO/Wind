#pragma once

#include "Graphics/Mesh.h"

namespace wind
{
    struct MeshDrawCommand
    {};

    class GPUScene
    {
    public:
        GPUScene(Device* device);

        void UploadMesh(Mesh& mesh);
        void ReleaseScene();

        const auto& GetMeshes() { return m_Meshes; }

    private:
        Device* m_Device;

        std::vector<MeshDrawCommand> m_MeshDrawCommands;
        std::vector<Mesh>            m_Meshes;
    };
} // namespace wind