#pragma once

#include "std.h"

#include "Backend/Command.h"

namespace wind {
enum class PassType { Graphics, Compute, AsyncCompute };

class ResourceRegistry;

class RenderGraphPassBase {
public:
    RenderGraphPassBase() = default;
    virtual ~RenderGraphPassBase() {}

    virtual void Execute(ResourceRegistry&, vk::CommandBuffer cb) noexcept {}

    PassType passType;
};

template <typename Data> 
class RenderGraphPass : public RenderGraphPassBase {
public:
    void Execute(ResourceRegistry&, vk::CommandBuffer cb) noexcept override {}
    RenderGraphPass(PassType type) { passType = type; }
    virtual ~RenderGraphPass() {}

    const Data& GetData() const noexcept { return m_data; }
    const Data* operator->() const { return &m_data; }

protected:
    Data m_data;
};

template <typename Data, typename ExecuteCallBack>
class RenderGraphPassConcrete : public RenderGraphPass<Data> {
public:
    explicit RenderGraphPassConcrete(ExecuteCallBack&& callback,
                                     PassType         type = PassType::Graphics)
        : RenderGraphPass<Data>(type), m_execCallBack(std::move(callback)) {}

    void Execute(ResourceRegistry& resourceRegistry, vk::CommandBuffer cb) noexcept override { 
        m_execCallBack(resourceRegistry, this->m_data, cb); 
    }

private:
    ExecuteCallBack m_execCallBack;
};
}; // namespace wind