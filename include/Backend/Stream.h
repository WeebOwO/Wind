#pragma once

#include <vector>

#include "Command.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "Core/NonCopy.h"

namespace wind
{
    class Device;

    enum class StreamMode : uint8_t
    {
        eImmdiately,
        eDeferred,
    };

    class CommandStream final : public Resource
    {
    public:
        NON_COPYABLE(CommandStream);

        CommandStream(Device* device, RenderCommandQueueType queueType, StreamMode mode);
        ~CommandStream() noexcept;

        vk::CommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

        void BeginRendering(const vk::RenderingInfo& info);
        void EndRendering();

        void BindPipeline(const Pipeline& pipeline);
        
        void DrawIndex(const DrawIndexCommand& command);
        void Flush();

        void RegisterWaitDependency(vk::Semaphore semaphore);
        void RegisterSignalDependency(vk::Semaphore semaphore);
        
        void BindVertexBuffer(BufferRef buffer, vk::DeviceSize offset, vk::DeviceSize range, uint32_t binding);
        void BindIndexBuffer(BufferRef buffer, vk::DeviceSize offset, vk::DeviceSize range, vk::IndexType indexType);

    private:
        void Init();
        void Destroy();

        StreamMode             m_StreamMode;
        vk::CommandBuffer      m_CommandBuffer;
        vk::CommandPool        m_CommandPool;
        RenderCommandQueueType m_QueueType;

        std::vector<vk::Semaphore> m_WaitQueue;
        std::vector<vk::Semaphore> m_SignalQueue;
        vk::SubmitInfo             m_SubmitInfo;
    };

    using CommandStreamRef = std::shared_ptr<CommandStream>;
} // namespace wind