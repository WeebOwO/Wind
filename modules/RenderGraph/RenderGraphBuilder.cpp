#include "RenderGraphBuilder.h"

#include "RenderGraph.h"

namespace wind
{
    void RenderGraphBuilder::AddPass(RenderGraphPass* pass)
    {
        // add pass
        m_RenderGraph.AddPass(pass);
    }

    void RenderGraphBuilder::Execute()
    {
        // execute
        m_RenderGraph.Compile();
        m_RenderGraph.Execute(m_CommandStream);
    }
} // namespace wind