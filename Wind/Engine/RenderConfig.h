#pragma once

#include "std.h"

namespace wind
{
    enum class RenderPath
    {
        None = 0,
        Defer
    };

    class RenderConfig
    {
    public:
        static constexpr uint32_t MRT_MAX_COUNT       = 10;
        static constexpr uint32_t MAX_FRAME_IN_FLIGHT = 2;
        uint32_t                  commandBufferPerThread;
        RenderPath                renderPath;
    };
} // namespace wind