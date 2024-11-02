#pragma once

#include "Core/Math.h"
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

        static std::vector<vk::VertexInputBindingDescription>   s_bindingDescriptions;
        static std::vector<vk::VertexInputAttributeDescription> s_attributeDescriptions;
    };

    // utility function to initialize vertex input state
    void InitVertexInputState(VertexLayoutType type, vk::PipelineVertexInputStateCreateInfo& vertexInputState);
} // namespace wind