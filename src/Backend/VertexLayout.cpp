#include "Backend/VertexLayout.h"

namespace wind
{
    std::vector<vk::VertexInputBindingDescription> StaticMeshVertex::s_bindingDescriptions = 
    {
        vk::VertexInputBindingDescription(0, sizeof(StaticMeshVertex), vk::VertexInputRate::eVertex)
    };

    std::vector<vk::VertexInputAttributeDescription> StaticMeshVertex::s_attributeDescriptions = 
    {
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(StaticMeshVertex, position))
    };

    void InitVertexInputState(VertexLayoutType type, vk::PipelineVertexInputStateCreateInfo& vertexInputState)
    {
        if (type == VertexLayoutType::StaticMesh)
        {
            vertexInputState.setVertexBindingDescriptions(StaticMeshVertex::s_bindingDescriptions)
                            .setVertexAttributeDescriptions(StaticMeshVertex::s_attributeDescriptions);
        }
    };
} // namespace wind