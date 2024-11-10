#include "Wind.h"

#include <memory>

#include "Core/Log.h"
#include "Backend/Swapchain.h"
#include "JobSystem/JobSystem.h"

namespace wind
{
    ClientApp::ClientApp(const CommandLineArguments& args, const WindowCreateInfo& windowCreateInfo) :
        m_commandLineArguments(args)
    {
        m_window = std::make_unique<Window>(windowCreateInfo.title, windowCreateInfo.width, windowCreateInfo.height);
    }

    ClientApp::~ClientApp() {}

    void ClientApp::Run()
    {
        WIND_CLIENT_INFO("ClientApp is running");

#if ENABLE_JOB_TEST
        JobSystem::RunTest();
#endif
        LoadScene();
        while (!m_window->ShouldClose())
        {
            m_renderer->BeginFrame();
            // main game loop here
            ExecutePlayerLoop();
            m_window->Update();

            m_renderer->EndFrame();
        }
    }

    void ClientApp::LoadScene() { WIND_CLIENT_INFO("Loading scene"); }

    void ClientApp::Init()
    {
        Log::Init();
        ParseCommandLine(m_commandLineArguments);
        RegisterWindow();

        // device extensions
        DeviceExtensions extensions = {.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, .enableValidationLayers = true};

        m_device = Device::Create(extensions, m_window.get());

        SwapchainCreateInfo swapchainCreateInfo = {m_window->GetWidth(), m_window->GetHeight(), 3, PresentMode::Fifo};
        m_renderer                              = Renderer::Craete(m_device.get(), swapchainCreateInfo);
        // init job system
        JobSystem::Init();
    }

    void ClientApp::RegisterWindow() { m_window->Init(); }

    void ClientApp::Shutdown()
    {
        WIND_CLIENT_INFO("ClientApp is shutting down");
        Log::Shutdown();
        JobSystem::Shutdown();
    }

    std::unique_ptr<Application> ClientApp::Create(const CommandLineArguments& args,
                                                   const WindowCreateInfo&     windowCreateInfo)
    {
        auto app = std::make_unique<ClientApp>(args, windowCreateInfo);
        app->Init();
        return std::move(app);
    }

    void ClientApp::ParseCommandLine(const CommandLineArguments& args)
    {
        WIND_CLIENT_INFO("Parsing command line arguments");
    }

    void ClientApp::ExecutePlayerLoop() 
    {
        // call all the player callbacks
        
    }
} // namespace wind