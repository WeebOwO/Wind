#include "PSOCache.h"

#include <memory>
#include <unordered_map>

#include <PipelineState.h>
#include <RefCntAutoPtr.hpp>

#include "ShaderCache.h"

namespace crychic
{
    static IRenderDevice*                                         renderDevice = nullptr;
    std::unordered_map<PsoStateID, RefCntAutoPtr<IPipelineState>> psoCache;

    static void CreateAllPso()
    {
        // Triangle PSO
        {
            GraphicsPipelineStateCreateInfo psoCI;
            RefCntAutoPtr<IPipelineState>   pso;
            psoCI.PSODesc.Name                                  = "Triangle PSO";
            psoCI.PSODesc.PipelineType                          = PIPELINE_TYPE_GRAPHICS;
            psoCI.GraphicsPipeline.NumRenderTargets             = 1;
            psoCI.GraphicsPipeline.RTVFormats[0]                = TEX_FORMAT_RGBA8_UNORM;
            psoCI.GraphicsPipeline.DSVFormat                    = TEX_FORMAT_D32_FLOAT;
            psoCI.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            psoCI.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
            psoCI.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

            psoCI.pVS = ShaderCache::GetShader(ShaderID::VS_BasePass);
            psoCI.pPS = ShaderCache::GetShader(ShaderID::PS_BasePass);

            renderDevice->CreateGraphicsPipelineState(psoCI, &pso);

            psoCache[PsoStateID::Triangle] = pso;
        }
    }

    IPipelineState* PSOCache::GetPSO(crychic::PsoStateID id) { return psoCache[id]; }

    void PSOCache::Init(IRenderDevice* device)
    {
        renderDevice = device;
        CreateAllPso();
    }

    void PSOCache::Destroy() {}
} // namespace crychic