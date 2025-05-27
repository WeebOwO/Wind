#pragma once 

#include <cstdint>

namespace wind 
{
    enum class Vertexlayout : uint8_t 
    {
        StaticMesh = 0,
        StaticMeshInstanced,
        SkinnedMesh,
    };
}