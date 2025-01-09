#pragma once

#include <atomic>

#include "Core/Log.h"

namespace wind
{
    // atomic counter for job system
    struct JobCounter
    {
        JobCounter() : m_count(0), m_taskComplete(false), m_used(false) {}

        void Increment()
        {
            m_count.fetch_add(1, std::memory_order_relaxed);
            m_taskComplete.store(false, std::memory_order_release);
        }

        void Decrement()
        {
            int currentCount = m_count.fetch_sub(1, std::memory_order_acq_rel);

            if (currentCount == 1)
            {
                m_taskComplete.store(true, std::memory_order_release);
                m_taskComplete.notify_all();
            }
        }

        void Wait()
        {
            WIND_CORE_INFO("Waiting...");
            while (!m_taskComplete.load(std::memory_order_acquire))
            {
                m_taskComplete.wait(false);
            }
            WIND_CORE_INFO("Done waiting");
        }

        void SetUsed(bool value) { m_used.store(value, std::memory_order_release); }

        bool IsUsed() const { return m_used.load(std::memory_order_acquire); }

    private:
        std::atomic<int>  m_count;
        std::atomic<bool> m_taskComplete;
        std::atomic<bool> m_used;
    };
} // namespace wind