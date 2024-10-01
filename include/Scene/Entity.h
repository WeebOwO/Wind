#pragma once

#include <entt/entt.hpp>

namespace wind
{
    class Entity
    {
    public:
        explicit Entity(entt::entity entity) : m_entity(entity) {}

        [[nodiscard]] entt::entity NativeHandle() const { return m_entity; }

    private:
        entt::entity m_entity;
    };
} // namespace wind