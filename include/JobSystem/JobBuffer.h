#pragma once

#include "JobSystem/JobSystem.h"
#include <cassert>

namespace wind
{
    // a job queue for job system
    // kind of ring buffer
    class JobBuffer
    {
    public:
        JobBuffer(size_t capacity) :
            m_capacity(capacity), m_jobs(new JobDeclaration[m_capacity + 1]), m_head(0), m_tail(0)
        {}

        ~JobBuffer() { delete[] m_jobs; }

        bool Push(const JobDeclaration& job)
        {
            if (IsFull())
            {
                return false;
            }

            m_jobs[m_tail] = job;
            m_tail         = (m_tail + 1) % m_capacity;
            return true;
        }

        JobDeclaration* Pop()
        {
            assert(!IsEmpty());

            JobDeclaration* job = &m_jobs[m_head];
            m_head              = (m_head + 1) % m_capacity;
            return job;
        }

        // need be thread safe
        JobDeclaration* Steal()
        {
            if (IsEmpty())
            {
                return nullptr;
            }

            size_t val = m_tail.fetch_sub(1) % m_capacity;
            return &m_jobs[val];
        }

        bool IsEmpty() { return m_head == m_tail; }

        bool IsFull() { return (m_tail + 1) % m_capacity == m_head; }

    private:
        friend class WorkerThread;

        size_t              m_capacity;
        size_t              m_head;
        std::atomic<size_t> m_tail;
        JobDeclaration*     m_jobs;
    };
} // namespace wind