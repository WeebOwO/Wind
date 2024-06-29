#include "EntryPoint.h"

class SandBox : public crychic::Application
{
public:
    SandBox() : Application({1280, 720, "SandBox"}) {}

    void Init() override
    {
        Application::Init(); // init the base system first
    }

    void Quit() override
    {
        Application::Quit(); // quit the base system first
    }
};

crychic::Application* crychic::CreateApplication(int argc, char** argv) { return new SandBox(); }

LRESULT CALLBACK MessageProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    Application* app = CreateApplication(__argc, __argv);
    app->ResigerWindow(hInstance, nShowCmd);
    app->Run();
    return 0;
}