#include "Material.h"

namespace wind
{
    Material::Material(const std::string& debugName,
                       ShadingModel       shadingModel,
                       BlendMode          blendMode,
                       RasterShader*      rasterShader) :
        m_debugName(debugName),
        m_shadingModel(shadingModel), m_blendMode(blendMode), m_rasterShader(rasterShader)
    {
        // we must init shader map and then init all material
        m_desc.rasterShader = rasterShader;
        m_desc.debugName    = debugName;
        m_desc.ShadingModel = shadingModel;
        m_desc.blendMode    = blendMode;
    }

    Ref<Material> Material::Create(const Desc& desc)
    {
        return ref::Create<Material>(desc.debugName, desc.ShadingModel, desc.blendMode, desc.rasterShader);
    }

    void MaterialManager::InitDefaultMaterial(ShaderMap& shaderMap)
    {
        // default lit
        Material::Desc desc {.debugName    = "default_lit",
                             .ShadingModel = ShadingModel::Lit,
                             .blendMode    = BlendMode::Opaque,
                             .rasterShader = shaderMap.GetRasterShader("BasePassShader").get()};

        m_materialCaches[desc.debugName] = Material::Create(desc);

        desc = {.debugName    = "default_unlit",
                .ShadingModel = ShadingModel::UnLit,
                .blendMode    = BlendMode::Opaque,
                .rasterShader = shaderMap.GetRasterShader("BasePassShader").get()};

        m_materialCaches[desc.debugName] = Material::Create(desc);
    }

    Ref<Material> MaterialManager::GetMaterial(const std::string& name)
    {
        assert(m_materialCaches.contains(name));
        return m_materialCaches[name];
    }
} // namespace wind