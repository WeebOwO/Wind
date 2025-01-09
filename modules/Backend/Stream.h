#pragma once

#include <vector>

#include "Buffer.h"
#include "Command.h"
#include "Pipeline.h"

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
        
        void BeginRecording();
        void EndRecording();

        void Reset();

        void BeginRendering(const vk::RenderingInfo& info);
        void EndRendering();

        void BindPipeline(const Pipeline& pipeline);

        void DrawIndex(const DrawIndexCommand& command);
        void Flush();

        void RegisterWaitDependency(vk::Semaphore semaphore);
        void RegisterWaitDependency(vk::PipelineStageFlags waitStage);
        void RegisterSignalDependency(vk::Semaphore semaphore);
        void RegisterSignalFence(vk::Fence fence);

        void BindVertexBuffer(BufferRef buffer, vk::DeviceSize offset, vk::DeviceSize range, uint32_t binding);
        void BindIndexBuffer(BufferRef buffer, vk::DeviceSize offset, vk::DeviceSize range, vk::IndexType indexType);

        void ClearColor(const ColorClearCommand& command);
        void ClearDepthStencil(const DepthStencilClearCommand& command);

        void TransitionImageLayout(const ImageLayoutTransitionCommand& command);

    private:
        void Init();
        void Destroy();

        StreamMode             m_StreamMode;
        vk::CommandBuffer      m_CommandBuffer;
        vk::CommandPool        m_CommandPool;
        RenderCommandQueueType m_QueueType;

        std::vector<vk::Semaphore>          m_WaitSemQueue;
        std::vector<vk::Semaphore>          m_SignalSemQueue;
        std::vector<vk::PipelineStageFlags> m_WaitStagesQueue;

        vk::SubmitInfo m_SubmitInfo;

        bool      m_NeedFence;
        vk::Fence m_Fence;
    };

    using CommandStreamRef = std::shared_ptr<CommandStream>;
} // namespace wind