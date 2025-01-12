#include "Wind.h"

using wind::ClientApp;

int main(int argc, char** argv)
{
    auto app =
        ClientApp::Create(wind::CommandLineArguments {argc, argv}, 
            wind::WindowCreateInfo {"Wind Engine", 1280, 720});
    app->Run();
    app->Shutdown();
    return 0;
}