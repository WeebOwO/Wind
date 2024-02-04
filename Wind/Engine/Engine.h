#pragma once

#include "std.h"

namespace wind
{
    class SceneRenderer;
    class Window;
    class Scene;
    class Renderer;

    class Engine
    {
    public:
        Engine(Scope<Window> window); // here use unique ptr to transfer ownership from editor to engine
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

        Scope<Window>                         m_window;
        std::vector<Scope<Scene>>             m_scenes;
        uint32_t                              m_activeSceneIndex;
        std::chrono::steady_clock::time_point m_lastTickTimePoint {std::chrono::steady_clock::now()};
        Scope<SceneRenderer>                  m_sceneRenderer;
    };
} // namespace wind