#pragma once

#include "RenderGraphBuilder.h"

namespace wind
{
    template<typename T, typename... Args>
    requires std::derived_from<T, RenderGraphPhase> 
    void RenderGraphBuilder::AddPhase(Args&&... args)
    {
        constexpr size_t alignment = alignof(T);
        // using placement new to construct the phase
        void* memory = m_Allocator->Allocate(sizeof(T), alignment);
        assert(memory != nullptr);
        T* phase = new (memory) T(m_RenderGraph, std::forward<Args>(args)...);
        m_RenderGraph.AddPhase(phase);
    }
} // namespace wind
