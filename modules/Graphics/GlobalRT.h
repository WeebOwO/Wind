#pragma once

#include <cstdint>

namespace wind 
{
    enum RTID : uint8_t
    {
        SceneColor = 0,
        SceneDepth = 1,
    };

    void InitGlobalRT(uint32_t width, uint32_t height);
}