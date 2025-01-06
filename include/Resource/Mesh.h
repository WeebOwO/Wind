#pragma once

#include "Core/Math.h"
#include "Core/Vector.h"

namespace wind
{
    struct Vertex
    {
        glm::vec3 position;
    };

    struct SubMesh
    {
        wind::vector<Vertex>   vertices;
        wind::vector<uint32_t> indices;
    };

    struct Mesh
    {
        std::vector<SubMesh> subMeshes;
    };
} // namespace wind