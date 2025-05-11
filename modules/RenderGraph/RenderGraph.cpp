#include "RenderGraph.h"

#include "Core/Log.h"
#include "Phase/RenderGraphPhase.h"
#include "RenderGraphBuilder.h"

namespace wind
{
    RenderGraph::RenderGraph(Device* device) : 
        m_Device(device),
        m_FrameAllocator(new LinearAllocator(1024 * 1024)),     // 1MB
        m_PersistentAllocator(new LinearAllocator(1024 * 1024)) // 1MB
    {
        m_FrameAllocator->Reset();
        m_PersistentAllocator->Reset();
    }

    RenderGraph::~RenderGraph() {}

    void RenderGraph::PrepareFrame(RenderGraphUpdateContext& context)
    {
        m_Context = context;
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
        }

        Compile();

        std::array<float, 4> red = {1.0f, 0.0f, 0.0f, 1.0f};

        for (auto& pass : m_Passes)
        {
            vk::CommandBuffer cmdBuffer = m_Context.commandStream->GetCommandBuffer();
            
            m_Device->BeginDebugRegion(cmdBuffer, pass->GetPassName().c_str(), red.data());
            pass->Execute(cmdBuffer);
            m_Device->EndDebugRegion(cmdBuffer);
        }
    }

    RenderGraphHandle RenderGraph::AllocRenderGraphResource(const RDGResourceDesc& resourceDesc)
    {
        // allocate a new resource
        VirtualResource* resource;
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
        
    }

    void RenderGraph::AfterExecute(PassNode* pass)
    {
        
    }
} // namespace wind