#pragma once

#include <glm/glm.hpp>

#include "Backend/Device.h"

namespace wind
{
    struct EulerRotation
    {
        float yaw;
        float pitch;
        float roll;
    };

    class Camera
    {
    public:
        Camera()  = default;
        ~Camera() = default;

        Camera(const float degFov, const float width, const float height, const float nearP, const float farP);

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetRotation(const EulerRotation& rotation) { m_RotationEuler = rotation; }

        void SetFov(float fov) { m_Fov = fov; }

        const glm::vec3&     GetPosition() const { return m_Position; }
        const EulerRotation& GetRotation() const { return m_RotationEuler; }

        float GetFov() const { return m_Fov; }

        void CreateRT(Device* device);
        void ReleaseRT(Device* device);

        void Update(float fs);

        Handle<GPUTexture> GetTextureHandle() const { return m_Texture; }

    private:
        void Init();

        EulerRotation m_RotationEuler;

        float m_NearPlane {1.0f};
        float m_FarPlane {20.0f};

        glm::vec3 m_RightDirection;

        glm::vec3 m_Position {0.0f, 0.0f, 0.0f};

        float              m_Fov;
        Handle<GPUTexture> m_Texture;

        glm::mat4 m_ViewProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        bool m_Dirty {true};
    };
} // namespace wind