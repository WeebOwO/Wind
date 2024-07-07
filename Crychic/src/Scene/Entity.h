#pragma once

#include "Scene.h"

namespace crychic
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity entity, Scene* scene) : m_entity(entity), m_scene(scene) {}

        ~Entity() = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template<typename T>
        T& GetComponent();

        template<typename T>
        void RemoveComponent();

        private:
        entt::entity   m_entity;
        Scene*         m_scene;

        inline static std::string name = "Unknown";
    };


    template<typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args)
    {
        return m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
    }

    template<typename T>
    T& Entity::GetComponent()
    {
        return m_scene->m_registry.get<T>(m_entity);
    }

    template<typename T>
    void Entity::RemoveComponent()
    {
        m_scene->m_registry.remove<T>(m_entity);
    }

} // namespace crychic