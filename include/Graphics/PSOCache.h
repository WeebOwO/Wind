#pragma once

#include "Backend/Device.h"

namespace wind
{
    class ShaderCache;

    enum PipelineID : uint32_t
    {
        Lighting,
        Count
    };

    class PSOCache
    {
    public:
        PSOCache(Device* device, ShaderCache* shaderCache);
        ~PSOCache();

        void Init();
        void Destroy();

    private:
        void CompileToPSO(PipelineID id);

        Device*      m_device;
        ShaderCache* m_shaderCache;
    };
} // namespace wind