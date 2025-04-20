#pragma once

#include <string>
#include <vector>

#include "Service.h"

namespace wind
{
    struct CommandLineArguments
    {
        int    argc;
        char** argv;
    };

    // define the interface for the application
    class Application : public std::enable_shared_from_this<Application>
    {
    public:
        virtual ~Application() = default;
        //  Application lifecycle
        virtual void Init();
        virtual void Shutdown();
        // Application loop
        virtual void Run() = 0;
        // Parse command line arguments
        virtual void ParseCommandLine(const CommandLineArguments& args) = 0;

        virtual void RegisterServices(Service* service);

        void TickAllServices();

        void PushFrameSignal(Signal signal);

    protected:
        // Application data
        CommandLineArguments m_CommandLineArgs;
        // Application state
        bool m_Running = true;
        // Application services
        std::vector<Service*> m_Services;
        std::vector<Signal> m_Signals;
    };

    template<typename T>
    requires std::derived_from<T, Application> class ApplicationGuard
    {
    public:
        ApplicationGuard(T* app) : m_App(app) { m_App->Init();}
        ~ApplicationGuard() { m_App->Shutdown(); }

        void Run() { m_App->Run(); }

    private:
        T* m_App;
    };

} // namespace wind