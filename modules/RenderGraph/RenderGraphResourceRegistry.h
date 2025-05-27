#pragma once

#include "VirtualResource.h"
#include "RenderGraphHandle.h"
#include "RenderGraph.h"  

namespace wind
{
    class RenderGraphResourceRegistry
    {
    public:
        RenderGraphResourceRegistry(RenderGraph* renderGraph) : m_RenderGraph(renderGraph) {}

        template<typename T>
        requires std::derived_from<T, VirtualResource> 
        T* GetResource(RenderGraphHandle handle)
        {
            if (handle.isInitialized() && m_RenderGraph)
            {
                return m_RenderGraph->GetResource(handle)->As<T>();
            }
            return nullptr;
        }

        template<typename T>
        requires std::derived_from<T, VirtualResource>
        T* GetResource(const std::string& name)
        {
            if (!m_RenderGraph) return nullptr;
            RenderGraphHandle handle = m_RenderGraph->GetBlackboard().Get(name);
            return GetResource<T>(handle);
        }

    private:
        RenderGraph* m_RenderGraph = nullptr;
    };
} // namespace wind