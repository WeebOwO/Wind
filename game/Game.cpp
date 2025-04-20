#include "Game.h"

#include <iostream>
#include "Core/GlobalContext.h"

namespace wind 
{
    GlobalContext* g_GlobalContext = nullptr;

    void Game::Init() 
    {
        ParseCommandLine(m_CommandLineArgs);
        g_GlobalContext = new GlobalContext();
        g_GlobalContext->Init();

        m_Renderer = std::make_unique<Renderer>();
        RegisterServices(m_Renderer.get());

        Application::Init();
    }

    void Game::Shutdown() 
    {
        Application::Shutdown();
        m_Renderer.reset();
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
                // set the working directory
                std::filesystem::current_path(path);
            }
        }
    }
}