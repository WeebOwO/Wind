#pragma once

#include <string>

namespace wind
{
    using RTID = std::string;

    struct GlobalRT
    {
        inline static RTID SceneColor = "SceneColor";
        inline static RTID BackBuffer = "BackBuffer";
    };
} // namespace wind