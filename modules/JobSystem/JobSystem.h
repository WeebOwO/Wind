#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "JobCounter.h"

namespace wind::job
{
    using JobEntry = std::function<void(void*)>;

    enum class JobPriority : uint8_t
    {
        Low = 0,
        Normal,
        High,
        Count
    };

    struct JobDeclaration
    {
        JobEntry    entry;
        JobPriority priority;
        void*       userData;
        JobCounter* counter;
        std::string jobname;

        void Execute()
        {
            entry(userData);
            if (counter)
            {
                counter->Decrement();
            }
        }
    };
    
    // initialize and shutdown the job system
    void Init();
    void Shutdown();

    // kick off a list of jobs
    void RunJob(const JobDeclaration& job);

    // allocate and free job counters
    // must be thread safe
    JobCounter* AllocateCounter();
    void        WaitAndFreeCounter(JobCounter* counter);
    // test inferface
    bool RunTest();
} // namespace wind