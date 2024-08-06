#pragma once

#include "Component.h"

#include "Core/UUID.h"

namespace crychic
{
    class IDComponent : public Component
    {
    public:
        UUID id;
    };
} // namespace crychic