#include "Scene.h"

namespace wind
{
    Scene::~Scene()
    {
        for (GameObject* go : m_GameObjects)
        {
            delete go;
        }
    }

    GameObject* Scene::CreateEntity(const std::string& name)
    {
        GameObject* go = new GameObject(name);
        m_GameObjects.push_back(go);
        return go;
    }

    void Scene::DestroyEntity(GameObject* entity)
    {
        auto it = std::find(m_GameObjects.begin(), m_GameObjects.end(), entity);
        if (it != m_GameObjects.end())
        {
            m_GameObjects.erase(it);
            delete entity;
        }
    }
} // namespace wind