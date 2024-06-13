#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "SubSystem.h"
#include "Window.h"

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
    // use the config info and init this application
    // this class represent the editor that user can interact with
    class Application final
    {
    public:
        explicit Application(const ApplicationDesc& config);
        ~Application();

        void Run();

        void Init();
        void Quit();

    private:
        std::unique_ptr<Window>                               m_window;
        std::array<std::unique_ptr<SubSystem>, System::Count> m_subSystems;
    };
} // namespace crychic