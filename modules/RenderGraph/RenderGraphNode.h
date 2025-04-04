#pragma once

#include <vector>

namespace wind
{
    using NodeID = size_t;

    enum class NodeType : uint8_t
    {
        Pass,
        Resource,
    };

    struct PassNode
    {
        NodeID              handle;
        std::vector<NodeID> dependencies;
        NodeType            type;
    };

    struct ResourceNode
    {
        
    };
} // namespace wind