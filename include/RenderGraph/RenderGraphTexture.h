#pragma once

#include <string>

namespace wind::rg
{
    struct RenderGraphTexture
    {
        struct Descriptor
        {
            int value;
        };

        Descriptor  desc;
        std::string name;
    };
} // namespace wind::rg