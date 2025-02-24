#pragma once

#include "Graphics/PsoCache.h"

#include "RenderGraphNode.h"

namespace wind
{
    class RenderGraph;

    class RenderGraphPass : public PassNode
    {
    public:
        RenderGraphPass(PSOCache* psoCache) : m_PsoCacheLibrary(psoCache) {};

        virtual void Draw(vk::CommandBuffer cmdBuffer) = 0;
        virtual ~RenderGraphPass()                     = default;

    protected:
        PSOCache* m_PsoCacheLibrary;
    };

} // namespace wind