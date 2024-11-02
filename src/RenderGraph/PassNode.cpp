#include "RenderGraph/PassNode.h"

namespace wind::rg
{
    RenderPassNode::RenderPassNode(RenderGraph* renderGraph, const std::string& name, RenderGraphPassBase* pass) :
        m_renderGraph(renderGraph), m_name(name), m_pass(pass)
    {}
} // namespace wind::rg