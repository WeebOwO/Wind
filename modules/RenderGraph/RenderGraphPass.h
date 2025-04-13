#pragma once

#include <any>
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

    enum PassType
    {
        RenderPass,
        ComputePass,
        AsyncComputePass,
    };

    class PassNode : public Node
    {
    public:
        PassNode(const std::string& name, PassType passType) : m_PassName(name), m_PassType(passType) {};

        void RegisterGraph(RenderGraph* renderGraph) { m_RenderGraph = renderGraph; }

        PassType           GetPassType() const { return m_PassType; }
        const std::string& GetPassName() const { return m_PassName; }

        virtual void RecordRenderGrpah(RenderGraph& renderGraph);
        virtual void Execute(vk::CommandBuffer cmdBuffer);

        virtual ~PassNode() = default;

    protected:
        RenderGraph* m_RenderGraph;
        std::string  m_PassName;
        PassType     m_PassType;
    };

    class RenderPassNode : public PassNode
    {
    public:
        RenderPassNode(const std::string& name) : PassNode(name, PassType::RenderPass) {};

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
            Attachments    attachments {};
            Viewport       viewPort {};
            vk::Rect2D     renderArea {};
            vk::ClearValue clearValue {};
            uint8_t        sampleCount = 1;
            uint8_t        layerCount  = 1;
        };

        template<typename T>
        T* GetUserData()
        {
            return std::any_cast<T>(&m_UserData);
        }

        void BeforeRendering(vk::CommandBuffer cmdBuffer);
        void AfterRendering(vk::CommandBuffer cmdBuffer);

    protected:
        std::any   m_UserData;
        Descriptor m_Descriptor {};
    };

} // namespace wind