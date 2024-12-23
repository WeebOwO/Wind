#pragma once

#include "Core/Component.h"
#include "Backend/Stream.h"
#include "Resource/Mesh.h"

namespace wind
{
    class MeshRenderer : public Component
    {
    public:
        MeshRenderer(const std::string& name) : Component(name) {}
        virtual ~MeshRenderer() = default;

        void Draw(CommandStreamRef commandStream);

    private:
        std::shared_ptr<Mesh> m_Mesh;
    };
}; // namespace wind