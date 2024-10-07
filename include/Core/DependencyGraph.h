#pragma once

#include <cstdint>

#include "Core/NonCopy.h"

namespace wind
{
    // dependency graph, come from filament engine
    // https://github.com/google/filament
    class DependencyGraph
    {
    public:
        NON_COPYABLE(DependencyGraph);

        DependencyGraph() noexcept;
        ~DependencyGraph() noexcept;

        using NodeID = uint32_t;

        class Node;

        struct Edge
        {
            NON_COPYABLE(Edge);
            const NodeID from;
            const NodeID to;

            Edge(DependencyGraph& graph, NodeID from, NodeID to) noexcept;
        };

        class Node
        {
        public:
            explicit Node(DependencyGraph& graph) noexcept;
        };

    private:
    };
} // namespace wind