#include "Wind.h"

#include <memory>

#include "Global.h"

#include "Backend/Swapchain.h"
#include "JobSystem/JobSystem.h"

namespace wind
{
    const std::string kMainSceneName = "MainScene";

    ClientApp::ClientApp(const CommandLineArguments& args, const WindowCreateInfo& windowCreateInfo) :
        m_CommandLineArguments(args)
    {
        m_Window = std::make_unique<Window>(windowCreateInfo.title, windowCreateInfo.width, windowCreateInfo.height);
    }

    ClientApp::~ClientApp() {}

    void ClientApp::Run()
    {
        WIND_CLIENT_INFO("ClientApp is running");

#if ENABLE_JOB_TEST
        JobSystem::RunTest();
#endif
        LoadScene();
        global::slangCompiler->Test();

        View view = {.camera = m_EditorCamera.get()};

        while (!m_Window->ShouldClose())
        {
            m_Renderer->BeginFrame();
            // main game loop here
            ExecutePlayerLoop();
            m_Window->Update();

            view.renderScene = global::sceneManager->GetActiveScene();
            m_Renderer->Render(view);

            m_Renderer->EndFrame();
        }
    }

    void ClientApp::LoadScene()
    {
        WIND_CLIENT_INFO("Loading scene");
        // load the scene
        SceneHandle sceneHandle = global::sceneManager->CreateScene({.sceneName = kMainSceneName});
    }

    void ClientApp::Init()
    {
        Log::Init();
        ParseCommandLine(m_CommandLineArguments);
        RegisterWindow();
        // device extensions
        DeviceExtensions extensions = {.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, .enableValidationLayers = true};

        m_Device = Device::Create(extensions, m_Window.get());

        SwapchainCreateInfo swapchainCreateInfo = {m_Window->GetWidth(), m_Window->GetHeight(), 3, PresentMode::Fifo};
        m_Renderer                              = Renderer::Craete(m_Device.get(), swapchainCreateInfo);
        // init job system
        JobSystem::Init();

        // global::Init();
        global::Init();

        m_EditorCamera = std::make_unique<Camera>();
    }

    void ClientApp::RegisterWindow() { m_Window->Init(); }

    void ClientApp::Shutdown()
    {
        WIND_CLIENT_INFO("ClientApp is shutting down");
        Log::Shutdown();
        JobSystem::Shutdown();
        global::Shutdown();
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