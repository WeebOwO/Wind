#include "Component.h"

#include "Resource/Mesh.h"

namespace wind
{
    void MeshComponent::OnCreate()
    {
        meshSource->InitRHI();
        isInitialized = true;
    }

    void MeshComponent::OnDestroy() { meshSource->ReleaseRHI(); }
}; // namespace wind