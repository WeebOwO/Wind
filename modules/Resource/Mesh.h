#pragma once

#include "Backend/Buffer.h"
#include "Backend/VertexLayout.h"
#include "Core/Math.h"
#include "Core/Vector.h"

namespace wind
{
    using Vertex = StaticMeshVertex;

    struct SubMesh
    {
        int                    materialIndex = -1;
        wind::vector<Vertex>   vertices;
        wind::vector<uint32_t> indices;
        BufferRef              vertexBuffer;
        BufferRef              indexBuffer;
    };

    struct Mesh
    {
        ~Mesh()
        {
            for (auto& subMesh : subMeshes)
            {
                subMesh.vertexBuffer.reset();
                subMesh.indexBuffer.reset();
            }
        }

        std::vector<SubMesh> subMeshes;
    };
} // namespace wind