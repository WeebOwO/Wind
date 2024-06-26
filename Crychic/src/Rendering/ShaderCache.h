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
        Count
    };

    class ShaderCache
    {
    public:
        static void Init(IRenderDevice* device);
        static void Destroy();

        static IShader* GetShader(ShaderID id);
    };
} // namespace crychic