#pragma once

#include <memory>

#include "Camera.h"

namespace wind
{
    class Scene;
    struct View
    {
        Camera* camera;
        Scene*  renderScene;
    };
} // namespace wind