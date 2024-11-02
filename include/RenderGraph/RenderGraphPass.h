#pragma once

#include "RenderGraphID.h"
#include <type_traits>

#include "Backend/Stream.h"
#include "Core/NonCopy.h"
#include "ResourceRegistry.h"

namespace wind::rg
{
    class RenderGraphPassExecutor
    {
    public:
        NON_COPYABLE(RenderGraphPassExecutor);

        RenderGraphPassExecutor() noexcept          = default;
        virtual ~RenderGraphPassExecutor() noexcept = default;

    protected:
        virtual void Execute(const ResourceRegistry& registry, CommandStream& stream) noexcept = 0;

    private:
        friend class RenderGraph;
        friend class PassNode;
    };

    class RenderGraphPassBase : public RenderGraphPassExecutor
    {
    public:
        using RenderGraphPassExecutor::RenderGraphPassExecutor;

        void            SetNode(PassNode* node) noexcept { m_node = node; }
        const PassNode& GetNode() noexcept { return *m_node; }

    private:
        friend class PassNode;
        friend class RenderGraph;
        friend class RenderPassNode;

        PassNode* m_node = nullptr;
    };

    template<typename Data>
    class RenderGraphPass : public RenderGraphPassBase
    {
    public:
        RenderGraphPass() noexcept          = default;
        virtual ~RenderGraphPass() noexcept = default;

        const Data& GetData() const noexcept { return m_data; }
        const Data& operator->() const noexcept { return m_data; }

    protected:
        Data m_data;

    private:
        friend class RenderGraph;
        void Execute(const ResourceRegistry& registry, CommandStream& stream) noexcept override {}
    };

    template<typename Data, typename Execute>
    requires std::is_invocable_v<Execute, const ResourceRegistry&, Data&, CommandStream&> 
    class RenderGraphPassConcrete
        : public RenderGraphPass<Data>
    {
        friend class RenderGraph;

        explicit RenderGraphPassConcrete(Execute&& execute) noexcept 
        : m_execute(std::forward<Execute>(execute)) {}

        void ExecuteImpl(const ResourceRegistry& registry, CommandStream& stream) noexcept
        {
            m_execute(registry, this->m_data, stream);
        }

        Execute m_execute;
    };
}; // namespace wind::rg