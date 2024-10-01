#pragma once

#include "Graphics/Camera.h"

namespace wind
{
    class View
    {
    public:
        View& SetCamera(Camera* camera);
        
    private:
        Camera* m_camera;
    };
} // namespace wind