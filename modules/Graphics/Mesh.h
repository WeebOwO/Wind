#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Backend/Device.h"

namespace wind
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    struct SubMesh
    {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
        Handle<GPUBuffer>     vertexBuffer;
        Handle<GPUBuffer>     indexBuffer;
    };

    struct Mesh
    {
        std::vector<SubMesh> subMeshes;
    };
} // namespace wind