#pragma once

#include <string>

#include "Backend/Define.h"
#include "Backend/Guard.h"
#include "Graphics/View.h"
#include "RenderGraphHandle.h"
#include "RenderGraphNode.h"
#include "Scene/Viewport.h"

namespace wind
{
    class RenderGraphUpdateContext;
    class RenderGraph;
    class RenderGraphBuilder;

    enum PassType
    {
        RenderPass,
        ComputePass,
        AsyncComputePass,
    };

    class PassNode : public Node
    {
    public:
        PassNode(const std::string& name, PassType passType) : m_PassName(name), m_PassType(passType)
        {
            nodeType = NodeType::Pass;
        };

        void RegisterGraph(RenderGraph* renderGraph) { m_RenderGraph = renderGraph; }

        PassType           GetPassType() const { return m_PassType; }
        const std::string& GetPassName() const { return m_PassName; }

        virtual void Setup(RenderGraphBuilder& renderGraph);
        virtual void Execute(vk::CommandBuffer cmdBuffer);

        virtual ~PassNode() = default;

    protected:
        RenderGraph* m_RenderGraph;
        std::string  m_PassName;
        PassType     m_PassType;
    };

    struct AttachmentInfo
    {
        RenderGraphHandle     handle;
        vk::ClearValue        clearValue;
        vk::AttachmentLoadOp  loadOp;
        vk::AttachmentStoreOp storeOp;
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
            std::vector<AttachmentInfo> renderTargets {};
            AttachmentInfo              depth {};
            AttachmentInfo              stencil {};
            Viewport                    viewPort {};
            vk::Rect2D                  renderArea {};
            vk::ClearValue              clearValue {};
            uint8_t                     sampleCount = 1;
            uint8_t                     layerCount  = 1;
        };

        void InitView(View* view) { m_View = view; }

        void BeginRendering(vk::CommandBuffer cmdBuffer);
        void EndRendering(vk::CommandBuffer cmdBuffer);

        void SetRenderTargets(const std::vector<AttachmentInfo>& renderTargets)
        {
            m_Descriptor.renderTargets = renderTargets;
        }

        void SetDepthAttachment(const AttachmentInfo& depth) { m_Descriptor.depth = depth; }
        void SetStencilAttachment(const AttachmentInfo& stencil) { m_Descriptor.stencil = stencil; }

    protected:
        Descriptor m_Descriptor {};
        View*      m_View;
    };

} // namespace wind