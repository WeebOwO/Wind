#include "SceneManager.h"

#include "Core/Log.h"
#include "Scene.h"

namespace wind
{
    Scene* SceneManager::GetScene(SceneHandle handle)
    {
        if (handle.index == INVALID_HANDLE)
        {
            WIND_CORE_ERROR("Invalid Scene Handle");
            return nullptr;
        }

        return m_Scenes[handle.index];
    }

    Scene* SceneManager::GetScene(const std::string& sceneName)
    {
        for (auto scene : m_Scenes)
        {
            if (scene->GetName() == sceneName)
            {
                return scene;
            }
        }

        WIND_CORE_ERROR("Scene not found");
        return nullptr;
    }

    SceneHandle SceneManager::CreateScene(const SceneCreateInfo& createInfo)
    {
        Scene* scene = new Scene();
        scene->SetName(createInfo.sceneName);

        auto sceneId = m_Scenes.size();
        m_Scenes.push_back(scene);

        SceneHandle handle = SceneHandle(sceneId);

        return handle;
    }

    void SceneManager::DestroyScene(SceneHandle& handle)
    {
        if (handle.index != INVALID_HANDLE)
        {
            WIND_CORE_ERROR("Invalid Scene Handle");
            return;
        }

        delete m_Scenes[handle.index];
        m_Scenes.erase(m_Scenes.begin() + handle.index);
    }

    Scene* SceneManager::GetActiveScene()
    {
        assert(m_Scenes.size() > 0);
        return m_Scenes[0];
    }

    SceneManager::~SceneManager()
    {
        for (auto scene : m_Scenes)
        {
            delete scene;
        }
    }
} // namespace wind