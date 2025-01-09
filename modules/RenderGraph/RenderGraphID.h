#pragma once

#include <cstdint>
#include <limits>

// come from filament engine
namespace wind::rg
{
    class RenderGraphHandle
    {
    public:
        using Index   = uint16_t;
        using Version = uint16_t;

        RenderGraphHandle(RenderGraphHandle const& rhs) noexcept = default;

        RenderGraphHandle& operator=(RenderGraphHandle const& rhs) noexcept = default;

        bool IsInitialized() const noexcept { return index != UNINITIALIZED; }

        operator bool() const noexcept { return IsInitialized(); }

        void Clear() noexcept
        {
            index   = UNINITIALIZED;
            version = 0;
        }

        bool operator<(const RenderGraphHandle& rhs) const noexcept { return index < rhs.index; }

        bool operator==(const RenderGraphHandle& rhs) const noexcept { return (index == rhs.index); }

        bool operator!=(const RenderGraphHandle& rhs) const noexcept { return !operator==(rhs); }

    private:
        template<typename T>
        friend class RenderGraphID;
        friend class BlackBoard;
        friend class RenderGraph;

        RenderGraphHandle() noexcept = default;
        explicit RenderGraphHandle(Index index) noexcept : index(index) {}

        static constexpr uint16_t UNINITIALIZED = std::numeric_limits<Index>::max();

        Index   index   = UNINITIALIZED;
        Version version = 0;
    };

    template<typename T>
    class RenderGraphID : public RenderGraphHandle
    {
    public:
        using RenderGraphHandle::RenderGraphHandle;

        RenderGraphID() noexcept = default;
        explicit RenderGraphID(RenderGraphHandle handle) noexcept : RenderGraphHandle(handle) {}
    };
} // namespace wind::rg