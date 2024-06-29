#pragma once

#include "Core/SubSystem.h"

namespace crychic
{
    class JobSystem : public SubSystem
    {
    public:
        MOVE_ONLY(JobSystem);

        JobSystem()           = default;
        ~JobSystem() override = default;

        void Init() override;
        void Quit() override;

        void Tick() override;

        void Clear();
    };

    extern JobSystem* g_jobSystem;
} // namespace crychic