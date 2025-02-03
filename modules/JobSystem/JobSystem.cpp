#include "JobSystem.h"

#include <Windows.h>
#include <condition_variable>
#include <memory.h>
#include <mutex>
#include <thread>

#include "Core/Log.h"
#include "JobBuffer.h"


namespace wind::job
{
    static size_t defaultBufferSize     = 1024;
    static size_t defaultJobCounterPool = 2048;

    static std::mutex              s_allocCounterMutex;
    static std::mutex              s_jobQueueMutex;
    static std::condition_variable s_cv;

    struct GlobalJobQueue
    {
    public:
        GlobalJobQueue()
        {

            for (int i = 0; i < (int)JobPriority::Count; ++i)
            {
                m_jobBuffers.push_back(std::make_unique<JobBuffer>(defaultBufferSize));
            }
        }

        bool HaveJobs()
        {
            for (int i = (int)JobPriority::Count - 1; i >= 0; --i)
            {
                if (!m_jobBuffers[i]->IsEmpty())
                {
                    return true;
                }
            }
            return false;
        };

        bool PushJob(JobDeclaration job)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutexes[int(job.priority)]);
                bool                        res = m_jobBuffers[int(job.priority)]->Push(job);
                if (res)
                {
                    job.counter->Increment();
                    s_cv.notify_one();
                }
                return res;
            }
        }

        JobDeclaration* QueueJob()
        {
            // get the job from the highest priority
            for (int i = int(JobPriority::Count) - 1; i >= 0; --i)
            {
                std::lock_guard<std::mutex> lock(m_mutexes[i]);
                if (!m_jobBuffers[i]->IsEmpty())
                {
                    return m_jobBuffers[i]->Pop();
                }
            }

            return nullptr;
        }

    private:
        friend class WorkerThread;
        std::mutex                              m_mutexes[int(JobPriority::Count)];
        std::condition_variable                 m_cv;
        std::vector<std::unique_ptr<JobBuffer>> m_jobBuffers;
    };

    static GlobalJobQueue s_jobQueue;

    struct WorkerThread
    {
        std::thread             thread;
        std::string             name;
        bool                    running;
        uint32_t                threadIndex;
        std::condition_variable cv;

        void Run()
        {
            while (running)
            {
                JobDeclaration* job = nullptr;
                {
                    std::unique_lock<std::mutex> lock(s_jobQueueMutex);
                    s_cv.wait(lock, [this] { return s_jobQueue.HaveJobs() || !running; });

                    if (running)
                    {
                        job = s_jobQueue.QueueJob();
                    }
                }

                if (job)
                {
                    job->Execute();
                }
            }
        }
    };

    struct JobCounterPool
    {
        std::vector<std::shared_ptr<JobCounter>> counters;
        size_t                                   capacity;

        JobCounterPool(size_t size) : capacity(size)
        {
            counters.reserve(size);
            for (size_t i = 0; i < size; ++i)
            {
                counters.emplace_back(std::make_shared<JobCounter>());
            }
        }

        JobCounter* Allocate()
        {
            for (auto& counter : counters)
            {
                if (!counter->IsUsed())
                {
                    counter->SetUsed(true);
                    return counter.get();
                }
            }
            return nullptr;
        }

        void Free(JobCounter* counter) { counter->SetUsed(false); }
    };

    struct Workers
    {
        std::vector<std::unique_ptr<WorkerThread>> workers;

        Workers(size_t size)
        {
            // create worker threads
            for (auto i = 0; i < size; ++i)
            {
                workers.push_back(std::make_unique<WorkerThread>());
                workers[i]->running = true;
                workers[i]->thread  = std::thread(&WorkerThread::Run, workers[i].get());
                // windows thread name
                HRESULT hr = SetThreadDescription(workers[i]->thread.native_handle(), L"WorkerThread");
                if (!hr)
                {
                    WIND_CORE_WARN("Failed to set thread name");
                }
            }
        }

        ~Workers()
        {
            for (auto i = 0; i < workers.size(); ++i)
            {
                workers[i]->running = false;
            }

            s_cv.notify_all();

            for (auto i = 0; i < workers.size(); ++i)
            {
                workers[i]->thread.join();
            }
        }

        WorkerThread* operator[](size_t index) { return workers[index].get(); }

        bool TryPushJob(const JobDeclaration& job) { return s_jobQueue.PushJob(job); }
    };

    static std::unique_ptr<Workers>        workerPool;
    static std::unique_ptr<JobCounterPool> jobCounterPool;

    static void InitJobWorkers(uint32_t numThreads) { workerPool = std::make_unique<Workers>(numThreads); };
    static void InitJobCounterPool() { jobCounterPool = std::make_unique<JobCounterPool>(defaultJobCounterPool); }
} // namespace wind::job

namespace wind::job
{
    void Init()
    {
        // initialize the job system
        size_t workerCount = std::thread::hardware_concurrency();
        // we will transfer main thread to worker thread
        // so we need to reduce the worker count by 1
        InitJobWorkers(workerCount - 1);
        InitJobCounterPool();
    }

    void Shutdown()
    {
        // shutdown the job system
        workerPool.reset();
        jobCounterPool.reset();
    }

    void RunJob(const JobDeclaration& job)
    {
        // run a single job
        if (!workerPool->TryPushJob(job))
        {
            WIND_CORE_WARN("Job buffer is full, job is not pushed");
        }
    }

    JobCounter* AllocateCounter()
    {
        // allocate a counter
        std::lock_guard<std::mutex> lock(s_allocCounterMutex);
        return jobCounterPool->Allocate();
    }

    void WaitAndFreeCounter(JobCounter* counter)
    {
        counter->Wait();
        std::lock_guard<std::mutex> lock(s_allocCounterMutex);
        jobCounterPool->Free(counter);
    }

    bool RunTest()
    {
        // run a test job
        JobDeclaration job;
        job.entry    = [](void* data) { WIND_CORE_INFO("Test job is running"); };
        job.priority = JobPriority::Normal;
        job.userData = nullptr;
        job.counter  = AllocateCounter();
        job.jobname  = "Test Job";

        RunJob(job);
        WaitAndFreeCounter(job.counter);
        return true;
    }
} // namespace wind::job