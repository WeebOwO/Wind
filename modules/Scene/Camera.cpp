#include "Camera.h"

namespace wind
{
    Camera::Camera(const float degFov, const float width, const float height, const float nearP, const float farP) :
        m_NearPlane(nearP),
        m_FarPlane(farP),
        m_Fov(degFov)
    {
        // set the camera
        m_Position = {0.0f, 0.0f, 0.0f};
    }

    void Camera::CreateRT(Device* device)
    {
        m_Texture = device->Create({
            .debugName = "CameraTexture",
            .extent    = {1024, 1024, 1},
            .format    = vk::Format::eR8G8B8A8Unorm,
            .usage     = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
            .mipMap    = false,
        });
    }

    void Camera::ReleaseRT(Device* device)
    {
        // release the texture
        if (m_Texture.IsNull())
        {
            return;
        }

        device->Free(m_Texture);
    }

    void Camera::Update(float ts) 
    {
        // update the camera
        if (m_Dirty)
        {
            
        }
    }
} // namespace wind