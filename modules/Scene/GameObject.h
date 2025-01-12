#pragma once

#include "COre/NonCopy.h"
#include "Component/Component.h"
#include "Core/UUID.h"
#include "Core/Vector.h"


namespace wind
{
    class Scene;

    class GameObject
    {
    public:
        NON_COPYABLE(GameObject)

        explicit GameObject() { m_UUID.GenerateUUID(); }
        GameObject(const std::string& name) : m_Name(name) { m_UUID.GenerateUUID(); }

        ~GameObject()
        {
            for (Component* component : m_Components)
            {
                component->OnDestroy();
                delete component;
            }
        }

        [[nodiscard]] UUID GetUUID() const { return m_UUID; }

        template<typename T>
        requires std::derived_from<T, Component> T* AddComponent()
        {
            // todo: use a pool allocator
            T* component = new T();
            m_Components.push_back(component);
            component->OnCreate();
            return component;
        }

        template<typename T>
        requires std::derived_from<T, Component> T* GetComponent()
        {
            for (Component* component : m_Components)
            {
                if (component->GetType() == T::GetType())
                {
                    return static_cast<T*>(component);
                }
            }
            return nullptr;
        }

    private:
        friend class Scene;

        std::string              m_Name;
        int                      m_Handle;
        UUID                     m_UUID;
        wind::vector<Component*> m_Components;
    };
} // namespace wind
