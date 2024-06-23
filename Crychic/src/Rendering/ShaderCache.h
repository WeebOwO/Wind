#pragma once

#include <RenderDevice.h>
#include <cstdint>

using namespace Diligent;
namespace crychic
{
    enum class ShaderID : uint32_t
    {
        VS_BasePass = 0,
        PS_BasePass,
    };

    class ShaderCache
    {
        static void Init(IRenderDevice* device);
        static void Destroy();

        IShader* GetShader(ShaderID id);
    };
} // namespace crychic