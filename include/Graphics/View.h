#pragma once

#include "Graphics/Camera.h"

namespace wind
{
    class Scene;
    struct View
    {
        Camera* renderCamera;
        Scene*  renderScene;
    };
} // namespace wind