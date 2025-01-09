#pragma once

#include "Enum.h"

namespace wind
{
    // auto generate string from enum using gpt
    template<typename T>
    std::string ConvertToSting(T value)
    {
        if constexpr (std::is_same_v<T, ResourceUsageType>)
        {
            switch (value)
            {
                case ResourceUsageType::Static:
                    return "Static";
                case ResourceUsageType::Dynamic:
                    return "Dynamic";
                default:
                    return "Unknown";
            }
        }
        else if constexpr (std::is_same_v<T, ResourceState>)
        {
            switch (value)
            {
                case ResourceState::UnDefined:
                    return "UnDefined";
                case ResourceState::VertexAndConstantBuffer:
                    return "VertexAndConstantBuffer";
                case ResourceState::IndexBuffer:
                    return "IndexBuffer";
                case ResourceState::RenderTarget:
                    return "RenderTarget";
                case ResourceState::UnorderedAccess:
                    return "UnorderedAccess";
                case ResourceState::DepthWrite:
                    return "DepthWrite";
                case ResourceState::DepthRead:
                    return "DepthRead";
                case ResourceState::NonPixelShaderResource:
                    return "NonPixelShaderResource";
                case ResourceState::PixelShaderResource:
                    return "PixelShaderResource";
                case ResourceState::ShaderResource:
                    return "ShaderResource";
                case ResourceState::IndirectArgument:
                    return "IndirectArgument";
                case ResourceState::CopyDest:
                    return "CopyDest";
                case ResourceState::CopySource:
                    return "CopySource";
                case ResourceState::GenericRead:
                    return "GenericRead";
                case ResourceState::Present:
                    return "Present";
                case ResourceState::Common:
                    return "Common";
                case ResourceState::AccelerationStructure:
                    return "AccelerationStructure";
                case ResourceState::ShaderRateSource:
                    return "ShaderRateSource";
                default:
                    return "Unknown";
            }
        }
        else
        {
            static_assert(false, "Unknown type");
        };
    }
} // namespace wind