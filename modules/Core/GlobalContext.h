#pragma once

namespace wind 
{
    struct GlobalContext 
    {
        void Init();
        void Shutdown();

        // global context data
        static constexpr int kMaxFramesInFlight = 2;
    };

    extern GlobalContext* g_GlobalContext;
}