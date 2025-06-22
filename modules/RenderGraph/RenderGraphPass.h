#pragma once

#include <string>
#include <unordered_map>

#include "Backend/Define.h"
#include "Backend/Guard.h"
#include "Graphics/View.h"
#include "Core/FastPropertyName.h"
#include "RenderGraphHandle.h"
#include "RenderGraphNode.h"
#include "RenderGraphResource.h"
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
        const std::string& GetPassName() const { return m_PassName.GetName(); }

        virtual void Setup(RenderGraphBuilder& renderGraph);
        virtual void Execute(RenderGraphUpdateContext& context);

        virtual ~PassNode() = default;

        // declare resource access
        void DeclareResourceAccess(RenderGraphHandle handle, 
                                 const ResourceState& state,
                                 const RDGSubresourceRange& range = RDGSubresourceRange()) {
            m_ResourceAccesses[handle] = {state, range};
        }

        const auto& GetResourceAccesses() const { return m_ResourceAccesses; }

    protected:
        RenderGraph* m_RenderGraph;
        FastPropertyName m_PassName;
        PassType     m_PassType;
        std::unordered_map<RenderGraphHandle, std::pair<ResourceState, RDGSubresourceRange>> m_ResourceAccesses;
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
            uint8_t                     sampleCount = 1;
            uint8_t                     layerCount  = 1;
        };

        void BeginRendering(vk::CommandBuffer cmdBuffer);
        void EndRendering(vk::CommandBuffer cmdBuffer);

        void SetRenderTargets(const std::vector<AttachmentInfo>& renderTargets)
        {
            m_Descriptor.renderTargets = renderTargets;
        }

        void SetDepthAttachment(const AttachmentInfo& depth) { m_Descriptor.depth = depth; }
        void SetStencilAttachment(const AttachmentInfo& stencil) { m_Descriptor.stencil = stencil; }

        // this function will be called during the render graph update phase
        void SetViewport(const Viewport& viewport) 
        { 
            m_Descriptor.viewPort = viewport;
            m_Descriptor.renderArea = vk::Rect2D {{0, 0}, viewport.width, viewport.height};
        }
        
        virtual void BakeRenderPipeline() {};

    protected:
        Descriptor m_Descriptor {};
    };

} // namespace wind