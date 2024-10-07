#include "Graphics/RenderConfig.h"

namespace wind::config
{
    RenderConfig s_RenderConfig;

    void SetRenderConfig(const RenderConfig& config) { s_RenderConfig = config; }

    const RenderConfig& GetRenderConfig() { return s_RenderConfig; }
} // namespace wind::config