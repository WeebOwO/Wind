#pragma once

#include <cstdint>
#include <memory>

#include "RenderCore.h"
#include "Rendering/Material.h"

namespace crychic
{
    struct SubMesh
    {
        uint32_t               materialIndex;
        RefCntAutoPtr<IBuffer> vertexBuffer;
        RefCntAutoPtr<IBuffer> indexBuffer;
    };

    struct MeshComponent
    {
        std::vector<SubMesh>  subMeshes;
        std::vector<Material> materials;
    };
} // namespace crychic