#pragma once

#include "Buffer.h"
#include <cstdint>

#include "Core/Handle.h"

namespace wind
{
    class Device;

    enum class RenderCommandQueueType : uint8_t
    {
        None         = 0x00,
        Copy         = 0x01,
        AsyncCompute = 0x02,
        Graphics     = 0x04,
        All          = Copy | AsyncCompute | Graphics
    };

    enum class RenderCommandType
    {
        Draw = 0,
        DrawIndirect,
        Dispatch,
        MeshDrawCommand,
        DispatchIndirect,
        UpdateBuffer,
        UpdateTexture,
        ClearColor,
        ClearDepthStencil,
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
    };

    struct DrawCommand : public CommandImpl<RenderCommandType::Draw, RenderCommandQueueType::Graphics>
    {
        Handle<Buffer> vertexBuffer;
        Handle<Buffer> indexBuffer;
        vk::Pipeline   pipeline;
        uint32_t       indexOffset    = 0;
        uint32_t       vertexOffset   = 0;
        uint32_t       instanceCount  = 1;
        uint32_t       instanceOffset = 0;
    };

    struct DrawIndexCommand : public CommandImpl<RenderCommandType::Draw, RenderCommandQueueType::Graphics>
    {
        uint32_t indexCount    = 0;
        uint32_t instanceCount = 0;
        uint32_t firstIndex    = 0;
        uint32_t vertexOffset  = 0;
        uint32_t firstInstance = 0;
    };

    struct DrawIndirectCommand : public CommandImpl<RenderCommandType::DrawIndirect, RenderCommandQueueType::Graphics>
    {
        uint32_t vertexCount   = 0;
        uint32_t instanceCount = 0;
        uint32_t firstVertex   = 0;
        uint32_t firstInstance = 0;
    };

    struct DispatchCommand : public CommandImpl<RenderCommandType::Dispatch, RenderCommandQueueType::AsyncCompute>
    {
        uint32_t groupCountX = 0;
        uint32_t groupCountY = 0;
        uint32_t groupCountZ = 0;
    };

    struct ColorClearCommand : public CommandImpl<RenderCommandType::ClearColor, RenderCommandQueueType::Graphics>
    {
        vk::ClearColorValue color;
        vk::Rect2D          renderArea;
    };

    struct DepthStencilClearCommand
        : public CommandImpl<RenderCommandType::ClearDepthStencil, RenderCommandQueueType::Graphics>
    {
        vk::ClearDepthStencilValue depthStencil;
        vk::Rect2D                 renderArea;
    };

    struct ImageLayoutTransitionCommand
        : public CommandImpl<RenderCommandType::UpdateTexture, RenderCommandQueueType::Graphics>
    {
        vk::ImageLayout           oldLayout;
        vk::ImageLayout           newLayout;
        vk::Image                 image;
        vk::ImageSubresourceRange subresourceRange;
        vk::ImageAspectFlags      aspectMask;
    };
} // namespace wind