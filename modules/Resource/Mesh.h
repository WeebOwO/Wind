#pragma once

#include "Backend/Buffer.h"
#include "Backend/VertexLayout.h"
#include "Core/Handle.h"
#include "Core/Math.h"
#include "Core/Vector.h"

namespace wind
{
    using Vertex = StaticMeshVertex;

    struct SubMesh
    {
        wind::vector<Vertex>   vertices;
        wind::vector<uint32_t> indices;
        Handle<Buffer>         vertexBuffer;
        Handle<Buffer>         indexBuffer;
    };

    struct Mesh
    {
        std::vector<SubMesh> subMeshes;
    };
} // namespace wind