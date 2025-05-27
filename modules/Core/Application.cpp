#include "Application.h"

#include <windows.h>
#include <dbghelp.h>
#include <tchar.h>

#pragma comment(lib, "Dbghelp.lib")

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

    
    LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionPointers)
    {
        HANDLE hFile =
            CreateFile("crash.dmp", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
            dumpInfo.ThreadId          = GetCurrentThreadId();
            dumpInfo.ExceptionPointers = pExceptionPointers;
            dumpInfo.ClientPointers    = FALSE;

            MiniDumpWriteDump(
                GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &dumpInfo, nullptr, nullptr);
            CloseHandle(hFile);
        }
        return EXCEPTION_EXECUTE_HANDLER;
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
        SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
        // tick all the services
        for (auto& service : m_Services)
        {
            service->Tick();
        }
    }
} // namespace wind