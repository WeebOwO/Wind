#pragma once

#include "Core/Component.h"

namespace wind
{
    class MeshRenderer : public Component
    {
    public:
        MeshRenderer(const std::string& name) : Component(name) {}
        virtual ~MeshRenderer() = default;

    private:
        // Mesh* m_mesh;
        // Material* m_material;
    };
}; // namespace wind