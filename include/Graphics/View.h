#pragma once

#include "Graphics/Camera.h"

namespace wind
{
    class Scene;
    class View
    {
    public:
        View& SetCamera(Camera* camera);
        View& SetScene(Scene* scene);

    private:
        Camera* m_camera;
        Scene*  m_scene;
    };
} // namespace wind