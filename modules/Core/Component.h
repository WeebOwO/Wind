#pragma once

#include "Core/Object.h"

namespace wind
{
    class Component : public NamedObject
    {
    public:
        Component(const std::string& name) : NamedObject(name) {}
        virtual ~Component() = default;
    };
} // namespace wind