#pragma once

#include <entt/entt.hpp>

#include "GameObject.h"

namespace wind
{
    enum SceneState
    {
        Invalid = 0,
        Loaded,
        Active,
        Unloaded
    };

    class Scene
    {
    public:
        Scene()          = default;
        virtual ~Scene();

        GameObject* CreateEntity(const std::string& name = "None");
        void        DestroyEntity(GameObject* entity);

        void               SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        auto GetGameObjects() { return m_GameObjects; }

    private:
        friend class GameObject;

        std::string              m_Name;
        SceneState               m_State = SceneState::Invalid;
        std::vector<GameObject*> m_GameObjects;
    };
} // namespace wind
