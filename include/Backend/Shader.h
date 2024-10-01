#pragma once

#include "Backend/Enum.h"
#include "Guard.h"
#include "Resource.h"

#include "Core/Vector.h"

namespace wind
{
    class Shader : public Resource
    {
    public:
        Shader(Device* device);
        virtual ~Shader() = default;

    protected:
        ShaderType m_type;
    };

    class RasterShader : public Shader
    {};

    class ComputeShader : public Shader
    {};
} // namespace wind