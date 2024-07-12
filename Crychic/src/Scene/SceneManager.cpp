#include "SceneManager.h"

#include "Entity.h"

namespace crychic
{
    SceneManager::SceneManager()
    {
        Scene::RegisterOnSceneActiveCallBack([this](Scene& scene, bool active) {
            if (active)
            {
                m_activeScene = scene.m_name;
            }
        });
    }

    Scene* SceneManager::CreateScene(const std::string& name)
    {
        if (m_scenes.find(name) != m_scenes.end())
        {
            return m_scenes[name];
        }
        else
        {
            m_scenes[name] = new Scene(name);
            return m_scenes[name];
        }
    }

    void SceneManager::RemoveScene(const std::string& name)
    {
        auto it = m_scenes.find(name);
        if (it != m_scenes.end())
        {
            m_scenes.erase(it);
        }
    }

    Scene* SceneManager::GetActiveScene() const { return m_scenes.at(m_activeScene); }

} // namespace crychic