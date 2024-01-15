#pragma once

#include <entt/entt.hpp>
#include <type_traits>

#include "ECS/Component.h"
#include "Scene/Scene.h"

namespace wind
{
    class GameObject
    {
    public:
        GameObject() = default;
        GameObject(entt::entity handle, Scene* scene) : m_enttHandle(handle), m_scene(scene) {}

        ~GameObject() {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must be component!");
            return m_scene->m_registry.emplace<T>(m_enttHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            return m_scene->m_registry.get<T>(m_enttHandle);
        }

        template<typename T>
        const T& GetComponent() const
        {
            return m_scene->m_registry.get<T>(m_enttHandle);
        }

        template<typename... T>
        bool HasComponent()
        {
            return m_scene->m_registry.all_of<T...>(m_enttHandle);
        }

        template<typename... T>
        bool HasComponent() const
        {
            return m_scene->m_registry.all_of<T...>(m_enttHandle);
        }

        template<typename... T>
        bool HasAny()
        {
            return m_scene->m_registry.any_of<T...>(m_enttHandle);
        }

        template<typename... T>
        bool HasAny() const
        {
            return m_scene->m_registry.any_of<T...>(m_enttHandle);
        }

        template<typename T>
        void RemoveComponent()
        {
            m_scene->m_registry.remove<T>(m_enttHandle);
        }

    private:
        entt::entity m_enttHandle;
        Scene*       m_scene;
    };
}; // namespace wind