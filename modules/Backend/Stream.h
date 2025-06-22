#pragma once

#include <functional>

#include "Buffer.h"
#include "Guard.h"
#include "Resource.h"

namespace wind
{
    enum class CommandQueueType : uint8_t
    {
        Graphics,
        Compute,
        AsyncCompute,
        Transfer,
        Count
    };

    enum class StreamMode : uint8_t
    {
        eImmdiately,
        eDeferred,
    };

    struct DrawCommand
    {
        Handle<GPUBuffer> vertexBuffer;
        Handle<GPUBuffer> indexBuffer;
        uint32_t          indexCount;
        uint32_t          instanceCount;
    };

    class CommandStream : public Resource
    {
    public:
        CommandStream(Device* device, CommandQueueType queueType, StreamMode mode);
        ~CommandStream() override = default;

        void InitRHI() override;
        void ReleaseRHI() override;

        template<typename Func>
        void RecordWork(Func&& func)
        {
            func(m_CommandBuffer);
        }

        void Submit();
        void Reset();

        void BeginRendering(vk::RenderingInfo& info);

        vk::CommandBuffer Begin();
        vk::CommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

        void Draw(const DrawCommand& draw);

    private:
        CommandQueueType  m_QueueType;
        StreamMode        m_Mode;
        vk::CommandBuffer m_CommandBuffer;
        vk::CommandPool   m_CommandPool;
        bool              m_IsRecording = false;
    };
} // namespace wind