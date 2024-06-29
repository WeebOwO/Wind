#include "JobSystem/JobSystem.h"

namespace crychic
{
    JobSystem* g_jobSystem = nullptr;

    void JobSystem::Init()
    {
        m_isInitialized = true;
        g_jobSystem     = this;
    }

    void JobSystem::Quit() {}

    void JobSystem::Tick() {}

    void JobSystem::Clear() {}
} // namespace crychic