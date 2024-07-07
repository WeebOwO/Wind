#include "Editor.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    crychic::Application* app = crychic::CreateApplication(__argc, __argv);
    app->RegisterWindow(hInstance, nShowCmd);
    app->Run();
    delete app;
    return 0;
}