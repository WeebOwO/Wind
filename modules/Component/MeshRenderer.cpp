#include "Component/MeshRenderer.h"

namespace wind
{
    void MeshRenderer::ExtractRenderContext()
    {
        
    }

    void MeshRenderer::OnCreate()
    {
        // Create the mesh
        RegisterType(GetType());
    }

    void MeshRenderer::OnDestroy()
    {
        // Destroy the mesh
        m_Mesh = nullptr;
    }
} // namespace wind