#pragma once

#include <cstdint>
#include <limits>

namespace wind
{
    class RenderGraphHandle
    {
    public:
        using Index   = uint16_t;
        using Version = uint16_t;

        template<typename T>
        friend class FrameGraphId;

        friend class Blackboard;
        friend class FrameGraph;
        friend class FrameGraphResources;
        friend class PassNode;
        friend class ResourceNode;

        // private ctor -- this cannot be constructed by users
        RenderGraphHandle() noexcept = default;
        explicit RenderGraphHandle(Index index) noexcept : index(index) {}

        // index to the resource handle
        static constexpr uint16_t UNINITIALIZED = std::numeric_limits<Index>::max();
        uint16_t                  index         = UNINITIALIZED; // index to a ResourceSlot
        Version                   version       = 0;

    public:
        RenderGraphHandle(RenderGraphHandle const& rhs) noexcept = default;

        RenderGraphHandle& operator=(RenderGraphHandle const& rhs) noexcept = default;

        bool isInitialized() const noexcept { return index != UNINITIALIZED; }

        operator bool() const noexcept { return isInitialized(); }

        void clear() noexcept
        {
            index   = UNINITIALIZED;
            version = 0;
        }

        bool operator<(const RenderGraphHandle& rhs) const noexcept { return index < rhs.index; }

        bool operator==(const RenderGraphHandle& rhs) const noexcept { return (index == rhs.index); }

        bool operator!=(const RenderGraphHandle& rhs) const noexcept { return !operator==(rhs); }
    };
} // namespace wind