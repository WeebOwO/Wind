#pragma once

#include "Path.h"

namespace wind 
{
    struct GlobalContext 
    {
        void Init();
        void Shutdown();

        // global context data
        static constexpr int kMaxFramesInFlight = 2;

        // game manager 
        PathManager pathManager;
    };

    extern GlobalContext* g_GlobalContext;
}