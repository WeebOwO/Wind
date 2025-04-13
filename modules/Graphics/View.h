#pragma once

#include "Backend/Guard.h"
#include "Scene/Camera.h"
#include "Scene/Viewport.h"

namespace wind
{
    struct View
    {
        Viewport   viewport;
        Camera     camera;
        vk::Rect2D scissor;
    };
} // namespace wind