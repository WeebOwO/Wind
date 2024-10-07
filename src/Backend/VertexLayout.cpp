#include "Backend/VertexLayout.h"

namespace wind
{
    void InitVertexInputState(VertexLayoutType type, vk::PipelineVertexInputStateCreateInfo& vertexInputState)
    {
        if (type == VertexLayoutType::StaticMesh)
        {
            // vertex binding
            vk::VertexInputBindingDescription bindingDescription;
            bindingDescription.setBinding(0)
                .setStride(sizeof(StaticMeshVertex))
                .setInputRate(vk::VertexInputRate::eVertex);

            // vertex attribute
            vk::VertexInputAttributeDescription attributeDescription;
            attributeDescription.setBinding(0)
                .setLocation(0)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(StaticMeshVertex, position));

            vertexInputState.setVertexBindingDescriptions(bindingDescription)
                .setVertexAttributeDescriptions(attributeDescription);
        }
    };
} // namespace wind