#pragma once

#include <string>

#include "Backend/Define.h"
#include "Backend/Guard.h"
#include "RenderGraphHandle.h"
#include "RenderGraphNode.h"
#include "Scene/Viewport.h"

namespace wind
{
    class RenderGraphUpdateContext;
    class RenderGraph;

    class RenderGraphPass : public Node
    {
    public:
        struct Attachments
        {
            union
            {
                RenderGraphHandle array[MAX_RENDER_TARGET_COUNT];
                struct
                {
                    RenderGraphHandle color[MAX_RENDER_TARGET_COUNT - 2];
                    RenderGraphHandle depth;
                    RenderGraphHandle stencil;
                };
            };
        };

        struct Descriptor
        {
            Attachments    attachments;
            Viewport       viewPort;
            vk::ClearValue clearValue;
            uint8_t        sampleCount;
            uint8_t        layerCount = 1;
        };

        RenderGraphPass(const std::string& name) : m_PassName(name) {};

        virtual void RecordRenderGrpah(RenderGraph& renderGraph);
        virtual void Execute(vk::CommandBuffer cmdBuffer);

        virtual ~RenderGraphPass() = default;

    protected:
        std::string m_PassName;
    };

} // namespace wind