#include "Graphics/View.h"

#include "Scene/Scene.h"

namespace wind
{
    View& View::SetCamera(Camera* camera)
    {
        m_camera = camera;
        return *this;
    }

    View& View::SetScene(Scene* scene)
    {
        m_scene = scene;
        return *this;
    }

    Scene* View::GetScene() const { return m_scene; }

    Camera* View::GetCamera() const { return m_camera; }
} // namespace wind