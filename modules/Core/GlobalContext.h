#pragma once

namespace wind::global 
{
    void Init();
    void Shutdown();

    constexpr int kMaxFramesInFlight = 2;
}