#pragma once

#include "RenderGraph.h"

namespace wind
{
    template<typename T, typename... Args>
    requires std::derived_from<T, RenderGraphPhase> 
    void RenderGraph::AddPhase(Args&&... args)
    {
        constexpr size_t alignment = alignof(T);
        void* memory = m_Allocator->Allocate(sizeof(T), alignment);
        assert(memory != nullptr);
        T* phase = new (memory) T(std::forward<Args>(args)...);
        m_Phases.push_back(phase); 
    }

    template<typename T, typename... Args>
    requires std::derived_from<T, RenderGraphPass>
    void RenderGraph::AddPass(Args&&... args)
    {
        constexpr size_t alignment = alignof(T);
        void* memory = m_Allocator->Allocate(sizeof(T), alignment);
        assert(memory != nullptr);
        T* pass = new (memory) T(std::forward<Args>(args)...);
        m_Passes.push_back(pass); 
    }
} // namespace wind
