#pragma once

#include "std.h"

#include "Engine/Window.h"

namespace wind
{
    struct ApplicationInfo
    {
        Window::WindowDesc windowDesc;
    };

    class Application
    {
    public:
        virtual void Init() {}
        virtual void Quit() {}
        virtual void Run() = 0;
    };

    Scope<Application> CreateClientApplication();
} // namespace wind