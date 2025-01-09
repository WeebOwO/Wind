#include "ResourceRegistry.h"

#include "PassNode.h"
#include "RenderGraph.h"

namespace wind::rg
{
    ResourceRegistry::ResourceRegistry(RenderGraph& rg, PassNode& node) noexcept 
    : m_rg(rg), m_node(node) {}

    std::string ResourceRegistry::GetPassName()
    {
        return m_node.GetPassName();
    }

    VirutalResource& ResourceRegistry::GetResource(RenderGraphHandle handle) const
    {
        return *(m_rg.GetResource(handle));
    }
} // namespace wind::rg