#pragma once

#include <unordered_map>

#include <entt/entt.hpp>

#include "Entity.h"

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
        virtual ~Scene() = default;

        Entity CreateEntity(const std::string& name = "None");
        void   DestroyEntity(Entity entity);

        void               SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

    private:
        friend class Entity;
        std::string m_Name;

        SceneState                           m_state = SceneState::Invalid;
        std::unordered_map<uint64_t, Entity> m_entities;
        entt::registry                       m_registry;
    };
} // namespace wind