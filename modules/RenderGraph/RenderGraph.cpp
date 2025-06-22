#include "RenderGraph.h"

#include "Core/Log.h"
#include "Phase/RenderGraphPhase.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphResourceRegistry.h"

namespace wind
{
    RenderGraph::RenderGraph(Device* device) : 
        m_Device(device),
        m_FrameAllocator(std::make_unique<LinearAllocator>(1024 * 1024)),     // 1MB
        m_PersistentAllocator(std::make_unique<LinearAllocator>(1024 * 1024)) // 1MB
    {
        m_FrameAllocator->Reset();
        m_PersistentAllocator->Reset();
        m_ResourceRegistry = std::make_unique<RenderGraphResourceRegistry>(this);
    }

    RenderGraph::~RenderGraph() 
    {
        // release the resources
        m_ResourceRegistry.reset();
        m_FrameAllocator.reset();
        m_PersistentAllocator.reset();
    }

    void RenderGraph::PrepareFrame(RenderGraphUpdateContext& context)
    {
        m_Context = context;
        m_Context.resourceRegistry = m_ResourceRegistry.get();
        m_FrameAllocator->Reset();
        m_Resources.clear();
        m_Passes.clear();
        m_ResourceNodes.clear();
    }

    void RenderGraph::Compile()
    {
        // compile all passes
    }

    void RenderGraph::Execute()
    {
        for (auto& pass : m_Passes)
        {
            RenderGraphBuilder builder(*this, pass);
            pass->Setup(builder);
            // default not culled
            pass->culled = false;
        }

        Compile();

        std::array<float, 4> red = {1.0f, 0.0f, 0.0f, 1.0f};

        for (auto* pass : m_Passes) {
            if (pass->culled) continue;

            BeforeExecute(pass);
            // collect resource transitions
            std::vector<ResourceTransition> transitions;
            // m_Device->BeginDebugRegion(m_Context.commandStream->GetCommandBuffer(), pass->GetPassName().c_str(), red.data());
            CollectTransitions(pass, transitions);
            
            // insert barriers
            InsertBarriers(m_Context.commandStream->GetCommandBuffer(), transitions);
            // execute pass
            pass->Execute(m_Context);
            
            // update global resource states
            for (const auto& [handle, access] : pass->GetResourceAccesses()) {
                m_GlobalResourceStates[handle] = access.first;
            }

            // m_Device->EndDebugRegion(m_Context.commandStream->GetCommandBuffer());
            AfterExecute(pass);
        }
    }

    RenderGraphHandle RenderGraph::AllocRenderGraphResource(const RDGResourceDesc& resourceDesc)
    {
        // allocate a new resource
        VirtualResource* resource = m_FrameAllocator->Construct<VirtualResource>(resourceDesc);
        m_Resources.push_back(resource);
        return RenderGraphHandle(m_Resources.size() - 1);
    }

    RenderGraphHandle RenderGraph::ImportRenderGraphResource(VirtualResource* resource)
    {
        // import a resource
        m_Resources.push_back(resource);
        return RenderGraphHandle(m_Resources.size() - 1);
    }

    RenderGraphHandle RenderGraph::WriteInternal(PassNode* pass, RenderGraphHandle handle)
    {
        // write a resource
        if (handle.isInitialized())
        {
            VirtualResource* resource = m_Resources[handle.index];
            resource->NeedByPass(pass);
        }
        else 
        {
            WIND_CORE_ERROR("RenderGraphHandle is not initialized");
        }

        return handle;
    }

    void RenderGraph::AddPassInternal(PassNode* pass)
    {
        // add a pass to the render graph
        pass->handle = m_Passes.size();
        m_Passes.push_back(pass);
        pass->RegisterGraph(this);
    }

    void RenderGraph::BeforeExecute(PassNode* pass)
    {
        // before execute the pass
        if (pass->GetPassType() == PassType::RenderPass) 
        {
            RenderPassNode* renderPass = static_cast<RenderPassNode*>(pass);
            renderPass->SetViewport(m_View->viewport);  
            renderPass->BeginRendering(m_Context.commandStream->GetCommandBuffer());
        }
    }

    void RenderGraph::AfterExecute(PassNode* pass)
    {
        if (pass->GetPassType() == PassType::RenderPass) 
        {
            RenderPassNode* renderPass = static_cast<RenderPassNode*>(pass);
            renderPass->EndRendering(m_Context.commandStream->GetCommandBuffer());
        }
    }

    void RenderGraph::CollectTransitions(PassNode* pass, std::vector<ResourceTransition>& transitions) {
        for (const auto& [handle, access] : pass->GetResourceAccesses()) {
            const auto& [newState, range] = access;
            const auto& currentState = m_GlobalResourceStates[handle];
            
            // check if need state transition
            if (!(currentState == newState)) {
                transitions.push_back({
                    handle,
                    currentState,
                    newState,
                    range
                });
            }
        }
    }

    void RenderGraph::InsertBarriers(vk::CommandBuffer cmdBuffer, 
                                   const std::vector<ResourceTransition>& transitions) {
        std::vector<vk::ImageMemoryBarrier> imageBarriers;
        std::vector<vk::BufferMemoryBarrier> bufferBarriers;

        for (const auto& transition : transitions) {
            auto* resource = GetResource(transition.handle);
            if (!resource) continue;

            if (resource->IsImage()) {
                vk::ImageMemoryBarrier barrier;
                barrier.oldLayout = transition.from.layout;
                barrier.newLayout = transition.to.layout;
                barrier.srcAccessMask = transition.from.accessFlags;
                barrier.dstAccessMask = transition.to.accessFlags;
                barrier.image = resource->GetImage();
                barrier.subresourceRange = {
                    transition.range.aspectMask,
                    transition.range.baseMipLevel,
                    transition.range.miplevels,
                    transition.range.baseArrayLayer,
                    transition.range.arrayLayers
                };
                imageBarriers.push_back(barrier);
            } else {
                vk::BufferMemoryBarrier barrier;
                barrier.srcAccessMask = transition.from.accessFlags;
                barrier.dstAccessMask = transition.to.accessFlags;
                barrier.buffer = resource->GetBuffer();
                barrier.offset = 0;
                barrier.size = VK_WHOLE_SIZE;
                bufferBarriers.push_back(barrier);
            }
        }

        if (!imageBarriers.empty() || !bufferBarriers.empty()) {
            cmdBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eAllCommands,
                vk::PipelineStageFlagBits::eAllCommands,
                vk::DependencyFlags(),
                {},  // memory barriers
                bufferBarriers,
                imageBarriers
            );
        }
    }
} // namespace wind