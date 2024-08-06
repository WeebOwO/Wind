#pragma once

#include <string>

#include "Component.h"

namespace crychic
{
    class TagComponent : public Component
    {
    public:
        TagComponent() = default;
        explicit TagComponent(const std::string& tag) : tag(tag) {}

        std::string tag;
    };
} // namespace crychic