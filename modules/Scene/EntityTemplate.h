#include "Entity.h"

#include "Scene.h"

namespace wind
{
    template<typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args)
    {
        return m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::RemoveComponent()
    {
        m_scene->m_registry.remove<T>(m_entity);
    }

    template<typename T>
    bool Entity::HasComponent() const
    {
        return m_scene->m_registry.all_of<T>(m_entity);
    }

    template<typename T>
    T& Entity::GetComponent()
    {
        return m_scene->m_registry.get<T>(m_entity);
    }
} // namespace wind