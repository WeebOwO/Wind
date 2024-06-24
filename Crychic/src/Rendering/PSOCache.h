#pragma once

#include <cstdint>

#include "RenderDevice.h"

using namespace Diligent;

namespace crychic
{
    enum class PsoStateID : uint32_t
    {
        Triangle = 0,
        Count
    };

    class PSOCache
    {
    public:
        static void Init(IRenderDevice* device);
        static void Destroy();

        static IPipelineState* GetPSO(PsoStateID id);
    };
} // namespace crychic