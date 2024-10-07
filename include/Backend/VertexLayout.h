#pragma once

#include "Core/Math.h"
#include "Core/Vector.h"
#include "Guard.h"

namespace wind
{
    enum VertexLayoutType
    {
        StaticMesh = 0,
        SkinnedMesh,
        InstancedStaticMesh,
    };

    struct StaticMeshVertex
    {
        glm::vec3 position;
    };

    // utility function to initialize vertex input state
    void InitVertexInputState(VertexLayoutType type, vk::PipelineVertexInputStateCreateInfo& vertexInputState);
} // namespace wind