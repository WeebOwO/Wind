#pragma once

#include <string>

#include "RenderGraphID.h"
#include "RenderGraphResource.h"

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

        template<typename PassType>
        requires std::is_base_of_v<PassNode, PassType>
        PassType* GetPass() const
        {
            return reinterpret_cast<PassType*>(&m_node);
        }

    private:
        VirutalResource& GetResource(RenderGraphHandle handle) const;

        RenderGraph& m_rg;
        PassNode&    m_node;
    };
} // namespace wind::rg