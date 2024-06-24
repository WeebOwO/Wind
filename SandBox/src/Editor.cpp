#include "EntryPoint.h"

class SandBox : public crychic::Application
{
public:
    SandBox()
        : Application({ 1280, 720, "SandBox" })
    {
    }

    void Init() override
    {
        Application::Init(); // init the base system first
    }

    void Quit() override
    {
        Application::Quit(); // quit the base system first
    }
    
};

crychic::Application* crychic::CreateApplication(int argc, char** argv)
{
    return new SandBox();
}