#pragma once

#include "Component.h"

#include "Core/Math.h"
#include "Core/Vector.h"

namespace wind
{
    class Transform : public Component
    {
    public:
        Transform(const std::string& name) : Component(name) {}
        virtual ~Transform() = default;

        // range-based for loop
        auto begin() { return m_Children.begin(); }
        auto end() { return m_Children.end(); }

    private:
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_scale;

        wind::vector<Transform> m_Children;
    };
} // namespace wind