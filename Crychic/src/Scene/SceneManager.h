#pragma once

#include "Core/Singleton.h"
#include "Scene.h"

namespace crychic
{
    class SceneManager : public Singleton<SceneManager>
    {
    public:
        SceneManager();
        ~SceneManager() = default;

        Scene* CreateScene(const std::string& name);
        void   RemoveScene(const std::string& name);

        Scene* GetActiveScene() const;

    private:
        std::string                             m_activeScene;
        std::unordered_map<std::string, Scene*> m_scenes;
    };
} // namespace crychic