#pragma once

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

    struct ApplicationDesc
    {
        uint32_t    width;
        uint32_t    height;
        std::string title;
    };

    class RenderSystem;
    class Window;
    class SubSystem;
    // use the config info and init this application
    // this class represent the editor that user can interact with
    class Application
    {
    public:
        explicit Application(const ApplicationDesc& config);
        ~Application();

        void Run();

        virtual void Init();
        virtual void Quit();

    private:
        std::unique_ptr<Window>                               m_window;
        std::array<std::unique_ptr<SubSystem>, System::Count> m_subSystems;
    };

    Application* CreateApplication(int argc, char** argv);
} // namespace crychic