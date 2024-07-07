#include "Scene.h"

#include "Component/IDComponent.h"
#include "Component/TagComponent.h"
#include "Entity.h"

namespace crychic
{
    static std::vector<Scene::OnSceneUpdateCallBack> s_onSceneUpdateCallBacks;

    Scene::Scene(const std::string& name) : m_name(name)
    {

    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = {m_registry.create(), this};
        entity.AddComponent<TagComponent>(name);
        auto idComponent            = entity.AddComponent<IDComponent>();
        m_entityMap[idComponent.id] = entity;
        return entity;
    }

    void Scene::RegisterOnSceneUpdateCallBack(const OnSceneUpdateCallBack& callBack)
    {
        s_onSceneUpdateCallBacks.push_back(callBack);
    }

    void Scene::Update(float ts)
    {
        for (auto& callBack : s_onSceneUpdateCallBacks)
        {
            callBack(*this, ts);
        }
    }

} // namespace crychic