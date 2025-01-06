#pragma once

#include <type_traits>

#include "Core/Log.h"
#include "Backend/Stream.h"
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
        void Execute(const ResourceRegistry& registry, CommandStream& stream) noexcept override \
        {
            WIND_CORE_INFO("This function should not be called");
        }
    };

    template<typename Data, typename ExecuteFunc>
    requires std::is_invocable_v<ExecuteFunc, const ResourceRegistry&, Data&, CommandStream&> 
    class RenderGraphPassConcrete
        : public RenderGraphPass<Data>
    {
        friend class RenderGraph;

        explicit RenderGraphPassConcrete(ExecuteFunc&& execute) noexcept 
        : m_execute(std::forward<ExecuteFunc>(execute)) {}

        void Execute(const ResourceRegistry& registry, CommandStream& stream) noexcept
        {
            m_execute(registry, this->m_data, stream);
        }

        ExecuteFunc m_execute;
    };
}; // namespace wind::rg