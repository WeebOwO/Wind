#pragma once

#include "Core/UUID.h"

#include <entt/entt.hpp>

namespace wind
{
    class Scene;

    class Entity
    {
    public:
        explicit Entity(entt::entity entity, Scene* scene) : m_entity(entity), m_scene(scene) { m_uuid.GenerateUUID(); }

        [[nodiscard]] entt::entity NativeHandle() const { return m_entity; }
        [[nodiscard]] UUID         GetUUID() const { return m_uuid; }

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template<typename T>
        void RemoveComponent();

        template<typename T>
        [[nodiscard]] bool HasComponent() const;

        template<typename T>
        [[nodiscard]] T& GetComponent();

    private:
        friend class Scene;

        entt::entity m_entity;
        UUID         m_uuid;
        Scene*       m_scene;
    };
} // namespace wind