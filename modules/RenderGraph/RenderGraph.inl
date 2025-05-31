#pragma once

#include "RenderGraph.h"

namespace wind
{
    template<typename T, typename... Args>
    requires std::derived_from<T, RenderGraphPhase> 
    void RenderGraph::AddPhase(Args&&... args)
    {
        // phase must be created in persistent memory
        T* phase = m_PersistentAllocator->Construct<T>(std::forward<Args>(args)...);
        m_Phases.push_back(phase); 
    }

    template<typename T>
    requires std::derived_from<T, PassNode>
    void RenderGraph::AddPass(T* pass)
    {
        AddPassInternal(pass);
    }
} // namespace wind
