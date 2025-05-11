#include "Game.h"

#include "Config.h"
#include "Core/GlobalContext.h"

namespace wind 
{
    GlobalContext* g_GlobalContext = nullptr;

    static std::filesystem::path workingDir;

    void Game::Init() 
    {
        g_GlobalContext = new GlobalContext();
        g_GlobalContext->Init();

        if (workingDir.empty()) 
        {
            workingDir = PROJECT_DIR;
        }

        g_GlobalContext->pathManager.MarkRootDir(workingDir);

        m_Renderer = std::make_unique<Renderer>();
        m_Camera   = std::make_unique<Camera>();

        m_Camera->CreateRT(m_Renderer->GetDevice());

        RegisterServices(m_Renderer.get());

        Application::Init();
    }

    void Game::Shutdown() 
    {
        m_Camera->ReleaseRT(m_Renderer->GetDevice());
        Application::Shutdown();
        g_GlobalContext->Shutdown();
    }

    void Game::Run() 
    {
        while(m_Running) 
        {
            // check for signals
            for (auto& signal : m_Signals) 
            {
                if (signal == Signal::NeedExit) 
                {
                    m_Running = false;
                }
            }

            m_Renderer->SetRenderCamera(m_Camera.get());

            // clear the signals
            m_Signals.clear();

            // tick the application
            TickAllServices();
        }
    }

    void Game::ParseCommandLine(const CommandLineArguments& args) 
    {
        for (int i = 0; i < args.argc; ++i) 
        {
            std::string arg(args.argv[i]);

            if (arg == "--exit") 
            {
                m_Running = false;
            }

            if (arg.find("--working-dir") != std::string::npos) 
            {
                // set the working directory
                std::string path = arg.substr(arg.find("=") + 1);
                workingDir = path;
            }
        }
    }
}