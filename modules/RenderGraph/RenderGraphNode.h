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

    struct Node
    {
        NodeID              handle;
        NodeType            nodeType;
        std::vector<NodeID> dependencies;
    };
} // namespace wind