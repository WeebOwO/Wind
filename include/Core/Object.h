#pragma once

#include <string>

namespace wind
{
    class Object
    {
    public:
        Object()          = default;
        virtual ~Object() = default;
    };

    class NamedObject : public Object
    {
    public:
        NamedObject(const std::string& name) : m_name(name) {}
        virtual ~NamedObject() = default;

        std::string GetName() const { return m_name; }

    private:
        std::string m_name;
    };
} // namespace wind