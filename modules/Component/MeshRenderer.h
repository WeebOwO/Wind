#pragma once

#include "Component.h"

#include "Backend/Stream.h"
#include "Resource/Mesh.h"

namespace wind
{
    class MeshRenderer : public Component
    {
    public:
        REGISTER_COMPONENT_TYPE("MeshRenderer")

        MeshRenderer() : Component(kName) {}
        MeshRenderer(const std::string& name) : Component(name) {}
        virtual ~MeshRenderer() = default;

        void ExtractRenderContext();
        void SetMesh(std::shared_ptr<Mesh> mesh) { m_Mesh = mesh; }

        void OnCreate() override;
        void OnDestroy() override;

        auto GetInternalMesh() { return m_Mesh; }

    private:
        std::shared_ptr<Mesh> m_Mesh;
    };
}; // namespace wind