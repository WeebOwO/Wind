#pragma once

#include "std.h"

#include <entt/entt.hpp>

#include "Core/UUID.h"
#include "Asset/Asset.h"
#include "Renderer/MeshPass.h"
#include "Scene/LightCaster.h"

namespace wind
{
    class GameObject;

    struct LightSceneInfo final
    {
        static constexpr size_t MaxDirectionalLights = 4;

        DirectionalLight        DirectionalLights[MaxDirectionalLights];
        std::vector<PointLight> PointLights;
        std::vector<SpotLight>  SpotLights;

        [[nodiscard]] auto GetPointLightsSize() const { return (uint32_t)(PointLights.size() * sizeof(PointLight)); }
        [[nodiscard]] auto GetSpotLightsSize() const { return (uint32_t)(SpotLights.size() * sizeof(SpotLight)); }
    };

    // game thread representation of a scene
    class Scene final : public Asset
    {
    public:
        using GameObjectMap = std::unordered_map<UUID, GameObject>;
        ~Scene();
        void Init();

        GameObject CreateGameObject(std::string name = {});
        void       Update();

    private:
        friend class GameObject;
        friend class SceneRenderer;

        entt::registry m_registry;
        LightSceneInfo m_lightSceneInfo;

        GameObjectMap m_gameObjectMap;

        std::array<MeshPass, MeshPassType::Count> m_meshPasses;

        bool m_isDirty = true;
    };
} // namespace wind
