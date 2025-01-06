#pragma once

#include <memory>

#include "Graphics/Camera.h"

namespace wind
{
    class Scene;
    struct View
    {
        Camera*                 camera;
        Scene*                  renderScene;
    };
} // namespace wind