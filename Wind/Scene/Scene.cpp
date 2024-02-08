#include "Scene.h"

#include "ECS/Component.h"
#include "ECS/Entity.h"
#include "Engine/RuntimeContext.h"
#include "Renderer/Material.h"
#include "Renderer/MeshPass.h"
#include "Resource/Mesh.h"

namespace wind
{
    GameObject Scene::CreateGameObject(std::string name)
    {
        auto  gameObject  = GameObject {m_registry.create(), this};
        auto& idComponent = gameObject.AddComponent<IDComponent>();
        gameObject.AddComponent<TagComponent>(name);

        idComponent.id                  = {};
        m_gameObjectMap[idComponent.id] = gameObject;
        m_isDirty                       = true;
        return gameObject;
    }

    void Scene::Update()
    {
        if (!m_isDirty)
            return;
        // clear up mesh pass
        std::for_each(m_meshPasses.begin(), m_meshPasses.end(), [](MeshPass& item) { item.Clear(); });

        auto view = m_registry.view<MeshComponent>();

        for (auto entity : view)
        {
            auto& meshComponent = view.get<MeshComponent>(entity);
            std::for_each(m_meshPasses.begin(), m_meshPasses.end(), [&](MeshPass& meshPass) {
                if (meshPass.filter(*meshComponent.meshSource->material))
                {
                    meshPass.staticMeshes.push_back(meshComponent.meshSource.get());
                }
            });
        }

        m_isDirty = false;
    }

    void Scene::Init()
    {
        // init the base pass
        auto& basePass  = m_meshPasses[MeshPassType::BasePass];
        basePass.type   = MeshPassType::BasePass;
        basePass.filter = [](const Material& material) {
            auto blendMode = material.desc().blendMode;
            return blendMode == BlendMode::Opaque;
        };
    }
} // namespace wind