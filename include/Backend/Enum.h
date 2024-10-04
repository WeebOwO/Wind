#pragma once

#include "Guard.h"
#include <cstdint>

namespace wind
{
    // mainly copy from raptor engine
    static const uint32_t kInvalidIndex = 0xffffffff;

    enum class ResourceUsageType
    {
        Static,
        Dynamic,
    };

    enum class ResourceState : uint32_t
    {
        UnDefined               = 0x0,
        VertexAndConstantBuffer = 0x1,
        IndexBuffer             = 0x2,
        RenderTarget            = 0x4,
        UnorderedAccess         = 0x8,
        DepthWrite              = 0x10,
        DepthRead               = 0x20,
        NonPixelShaderResource  = 0x40,
        PixelShaderResource     = 0x80,
        ShaderResource          = 0x100,
        IndirectArgument        = 0x200,
        CopyDest                = 0x400,
        CopySource              = 0x800,
        GenericRead             = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
        Present                 = 0x1000,
        Common                  = 0x2000,
        AccelerationStructure   = 0x4000,
        ShaderRateSource        = 0x8000,
    };

    enum class PipelineType : uint8_t
    {
        Graphics,
        Compute,
        RayTracing
    };

    enum class PresentMode
    {
        Immediate,
        Vsync,
        Mailbox,
        Fifo,
        FifoRelaxed
    };

    enum class ShaderType : uint8_t
    {
        Unknown,
        Vertex,
        Fragment,
        Compute,
        RayGen,
        Miss,
        ClosestHit,
        AnyHit,
        Intersection,
        Count
    };

    enum class ShadingModel : uint8_t
    {
        Unlit,
        PBR,
        Count
    };

    enum class PassType : uint8_t
    {
        Default = 0,
        GBuffer,
        Lighting,
        Count
    };
} // namespace wind