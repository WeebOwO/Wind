#pragma once

#include <glm/glm.hpp>

namespace wind
{
    class Camera
    {
    public:
        Camera()  = default;
        ~Camera() = default;

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
        void SetFov(float fov) { m_Fov = fov; }

        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::vec3& GetRotation() const { return m_Rotation; }
        float            GetFov() const { return m_Fov; }

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        float     m_Fov;
    };
} // namespace wind