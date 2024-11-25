#pragma once

#include <string>

#include "RenderGraph/RenderGraphID.h"
#include "RenderGraph/RenderGraphResource.h"

namespace wind::rg
{
    class PassNode;
    class RenderGraph;

    class ResourceRegistry
    {
    public:
        ResourceRegistry(RenderGraph& rg, PassNode& node) noexcept;
        ~ResourceRegistry() noexcept = default;

        std::string GetPassName();

        template<typename RESOURCE>
        const RESOURCE& Get(RenderGraphID<RESOURCE> resourceID) const
        {
            return static_cast<const Resource<RESOURCE>&>(GetResource(resourceID)).resource;
        }

    private:
        VirutalResource& GetResource(RenderGraphHandle handle) const;

        RenderGraph& m_rg;
        PassNode&    m_node;
    };
} // namespace wind::rg