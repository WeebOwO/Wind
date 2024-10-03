#pragma once

#include <string>

namespace wind
{
    struct MetaData
    {};

    // base class for all objects
    // store the runtime type information and some useful functions
    // also used for reflection and serialization

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