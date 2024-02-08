#include "std.h"

#include <GLFW/glfw3.h>
#include <Imgui/imgui.h>

#include "Engine/Engine.h"
#include "Engine/EntryPoint.h"
#include "Engine/Window.h"

class SimpleApp : public wind::Application
{
public:
    SimpleApp() {}

    void Run() override
    {
        wind::Window::WindowDesc  desc {.width = 1920, .height = 1080, .windowTitle = "Wind", .vsync = true};
        wind::Scope<wind::Window> window = wind::scope::Create<wind::Window>(desc);
        wind::Engine              engine(std::move(window));
        engine.Run();
    }
};

wind::Scope<wind::Application> wind::CreateClientApplication() { return scope::Create<SimpleApp>(); }