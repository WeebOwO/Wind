#pragma once

#include <cstdint>

namespace wind::config
{
    // store some global render config
    struct RenderConfig
    {
        uint32_t kmaxFramesInFlight = 2;
        uint32_t kmaxMRTAttachments = 8;
    };

    // store some global render config
    void                SetRenderConfig(const RenderConfig& config);
    const RenderConfig& GetRenderConfig();
} // namespace wind::config