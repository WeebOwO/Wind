#pragma once

#include <string>

namespace crychic
{
    class TagComponent
    {
    public:
        TagComponent() = default;
        explicit TagComponent(const std::string& tag) : tag(tag) {}

        std::string tag;
    };
} // namespace crychic