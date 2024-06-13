#pragma once

#include "Define.h"

namespace crychic
{
    class SubSystem
    {
    public:
        MOVE_ONLY(SubSystem);

        SubSystem()          = default;
        virtual ~SubSystem() = default;

        virtual void Init() = 0;
        virtual void Quit() = 0;
        virtual void Tick() {}

    protected:
        bool m_isInitialized = false;
    };
} // namespace crychic