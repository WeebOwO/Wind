#pragma once

#include "Backend/Device.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Graphics/Renderer.h"

namespace wind
{
    class Scene;

    struct WindowCreateInfo
    {
        const char* title;
        int         width;
        int         height;
    };

    class ClientApp : public Application
    {
    public:
        ClientApp(const CommandLineArguments& args, const WindowCreateInfo& windowCreateInfo);
        ~ClientApp();

        void Run() override;
        void Init() override;
        void Shutdown() override;

        static std::unique_ptr<Application> Create(const CommandLineArguments& args,
                                                   const WindowCreateInfo&     windowCreateInfo);

        void ExecutePlayerLoop();
        
    private:
        void LoadScene();
        void ParseCommandLine(const CommandLineArguments& args) override;
        void RegisterWindow();

        // Member variables
        const CommandLineArguments m_commandLineArguments;
        std::unique_ptr<Window>    m_window;
        std::unique_ptr<Device>    m_device;
        std::vector<Scene*>        m_scenes;
        std::unique_ptr<Renderer>  m_renderer;
    };
} // namespace wind