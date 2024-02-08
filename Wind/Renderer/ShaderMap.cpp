#include "ShaderMap.h"

#include "Backend/ComputeShader.h"
#include "Backend/RasterShader.h"

namespace wind
{
    ShaderMap::ShaderMap() {}

    void ShaderMap::CacheRasterShader(RasterShaderRef shader)
    {
        m_rasterShaderCache[shader->GetShaderName()] = shader;
    }

    void ShaderMap::CacheComputeShader(Ref<ComputeShader> shader)
    {
        m_computeShaderCache[shader->GetShaderName()] = shader;
    }
} // namespace wind