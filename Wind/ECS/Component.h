#pragma once

#include "Core/Log.h"
#include "std.h"

#include "Core/UUID.h"

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

} // namespace wind