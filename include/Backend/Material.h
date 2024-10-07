#pragma once

#include "Resource.h"
#include "Shader.h"

namespace wind
{
    // only use for lighting pass
    class MaterialInstance;

    class Material : public Resource
    {
    public:
        Material(Device* device, ShadingModel shadingModel, RasterShader* shader) :
            Resource(device, Tag::Material), m_shadingModel(shadingModel), shader(shader)
        {}

        void Init() override;
        void Destroy() override;

    private:
        ShadingModel  m_shadingModel;
        RasterShader* shader;
    };
}; // namespace wind

namespace wind::utils
{
    
}