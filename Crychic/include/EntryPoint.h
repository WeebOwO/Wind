#pragma once

#include "Application.h"

extern crychic::Application* crychic::CreateApplication(int argc, char** argv);

namespace crychic
{
    int Main(int argc, char** argv)
    {
        Application* app = CreateApplication(argc, argv);
        app->Run();
        delete app;
        return 0;
    }
} // namespace crychic

int main(int argc, char** argv) { crychic::Main(argc, argv); }