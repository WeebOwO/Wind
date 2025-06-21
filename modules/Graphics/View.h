#pragma once

#include "Backend/Guard.h"
#include "Scene/Camera.h"
#include "Scene/Viewport.h"

namespace wind
{
    struct View
    {
        Viewport   viewport;
        vk::Rect2D scissor;
        Camera*    camera;
    };
} // namespace wind