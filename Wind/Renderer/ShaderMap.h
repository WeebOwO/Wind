#pragma once

#include "std.h"

#include "Backend/Device.h"

namespace wind
{   
    class RasterShader;
    class ComputeShader;

    class ShaderMap
    {
    public:
        ShaderMap();

        // this interface is dump
        // todo: cache should never be known by end user
        void CacheRasterShader(RasterShaderRef shader);
        void CacheComputeShader(Ref<ComputeShader> shader);

        RasterShaderRef GetRasterShader(const std::string& shaderName) noexcept
        {
            assert(m_rasterShaderCache.contains(shaderName));
            return m_rasterShaderCache[shaderName];
        }

        Ref<ComputeShader> GetComputeShader(const std::string& shaderName) noexcept
        {
            assert(m_computeShaderCache.contains(shaderName));
            return m_computeShaderCache[shaderName];
        }

    private:
        std::unordered_map<std::string, RasterShaderRef>  m_rasterShaderCache;
        std::unordered_map<std::string, Ref<ComputeShader>> m_computeShaderCache;
    };
} // namespace wind