#pragma once

#include <string>
#include <unordered_map>

#include "Camera.h"

namespace wind
{
    enum class CameraType
    {
        GameView,
        SceneView,
    };

    class CameraManager
    {
    public:
        void RegisterCamera(const std::string& name, Camera* camera) { m_Cameras[name] = camera; }
        void UnregisterCamera(const std::string& name) { m_Cameras.erase(name); }

    private:
        std::unordered_map<std::string, Camera*> m_Cameras;
        Camera                                   m_GameViewCamera;
    };
} // namespace wind