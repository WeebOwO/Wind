#pragma once

#include "std.h"

#include "glm/glm.hpp"
#include "Core/Log.h"
#include "Core/UUID.h"
#include "ECS/Component.h"

namespace wind
{
    class StaticMesh;

    struct Component
    {
        virtual void OnCreate() {}
        virtual void OnDestroy() {}

        bool isInitialized = false;
    };

    struct IDComponent : public Component
    {
        UUID id;
    };

    struct TagComponent : public Component
    {
        std::string tag;

        TagComponent()                          = default;
        TagComponent(const TagComponent& other) = default;
        TagComponent(const std::string& tag) : tag(tag) {}

        operator std::string&() { return tag; }
        operator const std::string&() const { return tag; }
    };

    struct MeshComponent : public Component
    {
        MeshComponent() = default;
        MeshComponent(Ref<StaticMesh> mesh) : meshSource(mesh) {}

        void OnCreate() override;
        void OnDestroy() override;

        Ref<StaticMesh> meshSource;
    };

    struct TransformComponent : public Component
    {
        TransformComponent() = default;
        TransformComponent(const glm::vec3& position) : position(position) {}

        void OnCreate() override;
        void OnDestroy() override;

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale    = glm::vec3(1.0f);
    };
} // namespace wind