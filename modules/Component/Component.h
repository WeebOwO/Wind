#pragma once

#include "Core/Object.h"

namespace wind
{
    using TypeHash = size_t;

    struct ComponentType
    {
        TypeHash hash;

        bool operator==(const ComponentType& other) const { return hash == other.hash; }
    };

    class Component : public NamedObject
    {
    public:
        Component(const std::string& name) : NamedObject(name) {}
        virtual ~Component() = default;

        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate() {}

        ComponentType GetType() const { return m_Type; }
        void          RegisterType(ComponentType type) { m_Type = type; }

    private:
        ComponentType m_Type;
    };

#define REGISTER_COMPONENT_TYPE(Name) \
    inline static const std::string kName = #Name; \
    static ComponentType            GetType() \
    { \
        static ComponentType type = {.hash = std::hash<std::string> {}(kName)}; \
        return type; \
    }
}; // namespace wind