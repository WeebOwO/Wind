#pragma once

namespace wind
{
    struct CommandLineArguments
    {
        int    argc;
        char** argv;
    };

    // define the interface for the application
    class Application
    {
    public:
        virtual ~Application() = default;
        //  Application lifecycle
        virtual void Init() {}
        virtual void Shutdown() {}
        // Application loop
        virtual void Run() = 0;
        // Parse command line arguments
        virtual void ParseCommandLine(const CommandLineArguments& args) = 0;
    };
} // namespace wind