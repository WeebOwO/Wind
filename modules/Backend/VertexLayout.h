#pragma once

#include "Guard.h"
#include "Core/Math.h"

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
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<vk::VertexInputBindingDescription>   s_bindingDescriptions;
        static std::vector<vk::VertexInputAttributeDescription> s_attributeDescriptions;
    };

    // utility function to initialize vertex input state
    void InitVertexInputState(VertexLayoutType type, vk::PipelineVertexInputStateCreateInfo& vertexInputState);
} // namespace wind