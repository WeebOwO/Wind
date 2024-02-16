#pragma once

#include "std.h"

#include "Asset/Asset.h"
#include "Backend/Buffer.h"
#include "RenderResource.h"
#include "VertexFactory.h"

namespace wind
{
    class DeviceBuffer;
    class Material;

    struct MeshSource
    {
        std::vector<StaticMeshVertexFactory::Vertex> vertices;
        std::vector<StaticMeshVertexFactory::Index>  indices;

        DeviceBufferRef vertexBuffer;
        DeviceBufferRef indexBuffer;

        int materialIndex {-1};

        ~MeshSource() {}
    };

    class StaticMesh : public RenderResource
    {
    public:
        void InitRHI() override;
        void ReleaseRHI() override;

        MeshSource meshSource;
        Material*  material;
    };
} // namespace wind