#pragma once

#include <std.h>

#include "Engine/Application.h"

namespace wind
{
    class SceneRenderer;
    class Window;
    class Scene;
    class Renderer;

    class Engine
    {
    public:
        Engine(const ApplicationInfo& ApplicationInfo); // here use unique ptr to transfer ownership from editor to engine
        ~Engine();

        void Run();

    private:
        void Init();
        void PostInit();
        void Quit();

        void RenderTick(float delta);
        void LogicTick(float delta);

        void  LoadScene();
        float CalcDeltaTime();

        ApplicationInfo                       m_applicaitonInfo;
        Scope<Window>                         m_window;
        Scope<SceneRenderer>                  m_sceneRenderer;
        std::vector<Scope<Scene>>             m_scenes;
        uint32_t                              m_activeSceneIndex;
        std::chrono::steady_clock::time_point m_lastTickTimePoint {std::chrono::steady_clock::now()};
    };
} // namespace wind