#pragma once

#include "Engine/Application.h"
#include "Engine/Engine.h"

extern wind::Scope<wind::Application> CreateClientApplication();

namespace wind
{
    int GuardedMain(int argc, char** argv)
    {
        auto app = CreateClientApplication();
        app->Run();
        return 0;
    }
} // namespace wind

int main(int argc, char** argv) { return wind::GuardedMain(argc, argv); }
