#pragma once

#include "Graphics/PsoCache.h"

#include "RenderGraphNode.h"

namespace wind
{
    class RenderGraphPass : public PassNode
    {
    public:
        RenderGraphPass(PSOCache* psoCache) : m_PsoCacheLibrary(psoCache) {};

        virtual void Setup() = 0;
        virtual void Draw(vk::CommandBuffer cmdBuffer)  = 0;
        virtual ~RenderGraphPass()                      = default;

    protected:
        PSOCache* m_PsoCacheLibrary;
    };

} // namespace wind