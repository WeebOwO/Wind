#pragma once

#include "Core/Singleton.h"
#include "Scene.h"

namespace crychic
{
    class SceneManager : public Singleton<SceneManager>
    {
    public:
        SceneManager()  = default;
        ~SceneManager() = default;

        Scene* GetOrCreate(const std::string& name);
        void   RemoveScene(const std::string& name);

    private:
        std::unordered_map<std::string, Scene*> m_scenes;
    };
} // namespace crychic