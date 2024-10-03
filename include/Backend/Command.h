#pragma once

#include <cstdint>

namespace wind
{
    class Device;

    enum class RenderCommandQueueType : uint8_t
    {
        None     = 0x00,
        Copy     = 0x01,
        Compute  = 0x02,
        Graphics = 0x04,
        All      = Copy | Compute | Graphics
    };

    enum class RenderCommandType
    {
        Draw = 0,
        DrawIndirect,
        Dispatch,
        DispatchIndirect,
        UpdateBuffer,
        UpdateTexture,
        Count
    };

    struct Command
    {
        RenderCommandType type = RenderCommandType::Count;
    };

    template<RenderCommandType CommandType, RenderCommandQueueType QueueType>
    struct CommandImpl : public Command
    {
        static constexpr RenderCommandType      type      = CommandType;
        static constexpr RenderCommandQueueType queueType = QueueType;

        CommandImpl() { this->type = type; }
    };

    struct DrawCommand : public CommandImpl<RenderCommandType::Draw, RenderCommandQueueType::Graphics>
    {
        uint32_t vertexCount   = 0;
        uint32_t instanceCount = 0;
        uint32_t firstVertex   = 0;
        uint32_t firstInstance = 0;
    };

    struct DrawIndirectCommand : public CommandImpl<RenderCommandType::DrawIndirect, RenderCommandQueueType::Graphics>
    {
        uint32_t vertexCount   = 0;
        uint32_t instanceCount = 0;
        uint32_t firstVertex   = 0;
        uint32_t firstInstance = 0;
    };

    struct DispatchCommand : public CommandImpl<RenderCommandType::Dispatch, RenderCommandQueueType::Compute>
    {
        uint32_t groupCountX = 0;
        uint32_t groupCountY = 0;
        uint32_t groupCountZ = 0;
    };
} // namespace wind