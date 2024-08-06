#pragma once

#include "Component.h"

#include "RenderCore.h"

namespace crychic
{
    struct SubMesh
    {
        uint32_t               materialIndex;
        RefCntAutoPtr<IBuffer> vertexBuffer;
        RefCntAutoPtr<IBuffer> indexBuffer;
    };

    class StaticMeshRenderer : public Component
    {
    public:
        
    private:
        std::vector<SubMesh> m_subMeshes;
    };
}