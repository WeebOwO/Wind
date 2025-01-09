#pragma once

#include "Application.h"
#include "Window.h"
#include "Backend/Device.h"
#include "Graphics/Camera.h"
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
        const CommandLineArguments m_CommandLineArguments;
        std::unique_ptr<Window>    m_Window;
        std::unique_ptr<Device>    m_Device;
        std::unique_ptr<Renderer>  m_Renderer;
        std::unique_ptr<Camera>    m_EditorCamera;
    };
} // namespace wind