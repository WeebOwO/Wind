#include "JobSystem/JobSystem.h"

namespace crychic
{
    JobSystem* g_jobSystem = nullptr;

    void JobSystem::Init()
    {
        m_isInitialized = true;
        g_jobSystem = this;
    }

    void JobSystem::Quit() {}

    void JobSystem::Tick() { m_executor.run(m_taskflow).wait(); }

    void JobSystem::Clear() { m_taskflow.clear(); }
} // namespace crychic