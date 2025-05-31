#pragma once

#include "RenderGraph.h"
#include "RenderGraphHandle.h"
#include "RenderGraphPass.h"
#include "RenderGraphResource.h"
#include "VirtualResource.h"


namespace wind
{
    class RenderGraph;

    class RenderGraphBuilder
    {
    public:
        RenderGraphBuilder(RenderGraph& renderGraph, PassNode* pass);

        RenderGraphHandle Create(const RDGResourceDesc& resourceDesc);
        RenderGraphHandle Import(VirtualResource* resource, const std::string& name);
        RenderGraphHandle GetResourceHandle(const std::string& name);

        // Convenience methods for common resource states
        void RenderTarget(RenderGraphHandle handle, const RDGSubresourceRange& range = RDGSubresourceRange())
        {
            Write(handle, ResourceStates::ColorAttachment, range);
        }

        void DepthStencil(RenderGraphHandle handle, const RDGSubresourceRange& range = RDGSubresourceRange())
        {
            Write(handle, ResourceStates::DepthStencilAttachment, range);
        }

        void Sample(RenderGraphHandle handle, const RDGSubresourceRange& range = RDGSubresourceRange())
        {
            Read(handle, ResourceStates::ShaderRead, range);
        }

    private:
        // Resource access methods
        void Read(RenderGraphHandle          handle,
                  const ResourceState&       state = ResourceStates::ShaderRead,
                  const RDGSubresourceRange& range = RDGSubresourceRange())
        {
            m_Pass->DeclareResourceAccess(handle, ResourceStates::ShaderRead, range);
        }

        void Write(RenderGraphHandle          handle,
                   const ResourceState&       state,
                   const RDGSubresourceRange& range = RDGSubresourceRange())
        {
            m_Pass->DeclareResourceAccess(handle, state, range);
            m_RenderGraph.WriteInternal(m_Pass, handle);
        }

        RenderGraph& m_RenderGraph;
        PassNode*    m_Pass;
    };
} // namespace wind