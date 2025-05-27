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
        }

        Compile();

        std::array<float, 4> red = {1.0f, 0.0f, 0.0f, 1.0f};

        for (auto& pass : m_Passes)
        {          
            m_Device->BeginDebugRegion(m_Context.cmdBuffer, pass->GetPassName().c_str(), red.data());
            pass->Execute(m_Context);
            m_Device->EndDebugRegion(m_Context.cmdBuffer);
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