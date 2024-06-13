#pragma once

#include "taskflow.hpp"

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

        template<typename... T>
        auto Emplace(T&&... t)
        {
            return m_taskflow.emplace(std::forward<T...>(t...));
        }

    private:
        tf::Executor m_executor;
        tf::Taskflow m_taskflow;
    };

    extern JobSystem* g_jobSystem;
} // namespace crychic