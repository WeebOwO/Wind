#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <JobSystem/JobCounter.h>

namespace wind
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

    class JobSystem
    {
    public:
        // initialize and shutdown the job system
        static void Init();
        static void Shutdown();

        // kick off a list of jobs
        static void RunJob(const JobDeclaration& job);

        // allocate and free job counters
        // must be thread safe
        static JobCounter* AllocateCounter();
        static void        WaitAndFreeCounter(JobCounter* counter);
        // test inferface
        static bool RunTest();
    };

} // namespace wind