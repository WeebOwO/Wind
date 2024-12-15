#pragma once

#include "Backend/Command.h"
#include "Backend/Pipeline.h"
#include "vulkan/vulkan_handles.hpp"
#include <queue>
#include <vector>

#include "Command.h"
#include "Core/NonCopy.h"
#include "Resource.h"

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