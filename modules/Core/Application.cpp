#include "Application.h"

namespace wind 
{
    void Application::RegisterServices(Service* service)
    {
        // register the service
        service->BindApplication(this);
        m_Services.push_back(service);
    }

    void Application::ParseCommandLine(const CommandLineArguments& args)
    {
        // parse the command line arguments
        m_CommandLineArgs = args;
    }

    void Application::Init()
    {
        // initialize the application
        for (auto& service : m_Services)
        {
            service->Init();
        }
    }

    void Application::Shutdown()
    {
        // shutdown the application
        for (auto& service : m_Services)
        {
            service->Shutdown();
        }
    }

    void Application::PushFrameSignal(Signal signal)
    {
        // push the signal to the application
        m_Signals.push_back(signal);
    }

    void Application::TickAllServices() 
    {
        // tick all the services
        for (auto& service : m_Services)
        {
            service->Tick();
        }
    }  
}