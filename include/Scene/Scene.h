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

    private:
        friend class Entity;

        SceneState                           m_state = SceneState::Invalid;
        std::unordered_map<uint64_t, Entity> m_entities;
        entt::registry                       m_registry;
    };
} // namespace wind