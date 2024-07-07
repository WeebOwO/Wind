#pragma once

#include <Windows.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace crychic
{
    enum System
    {
        Job = 0,
        Render,
        Count
    };

    enum class ApplicationType : uint32_t
    {
        Editor = 0,
        Game,
    };

    struct ApplicationConfig
    {
        uint32_t        width;
        uint32_t        height;
        std::string     title;
        ApplicationType type;
    };

    class RenderSystem;
    class Window;
    class SubSystem;
    // use the config info and init this application
    // this class represent the editor that user can interact with
    class Application
    {
    public:
        explicit Application(const ApplicationConfig& config);
        ~Application();

        void Run();

        virtual void Init();
        virtual void Quit();
        virtual void Update() {}

        void LoadScene(const std::string& path);

        void RegisterWindow(HINSTANCE hinstance, int nShowCmd);

    private:
        ApplicationConfig                                     m_config;
        std::unique_ptr<Window>                               m_window;
        std::array<std::unique_ptr<SubSystem>, System::Count> m_subSystems;
    };

    Application* CreateApplication(int argc, char** argv);
} // namespace crychic