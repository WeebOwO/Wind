#pragma once

#include "Backend/Buffer.h"
#include "Resource/VertexFactory.h"

namespace wind
{
    class Material;
    class StaticMesh;
    class MeshSource;
    class MeshPass;

    enum MeshPassType : uint8_t
    {
        BasePass = 0,
        Count
    };

    template<typename T>
    class PerpassData
    {
    public:
        T&   operator[](MeshPassType pass) { return m_data[uint32_t(pass)]; }
        void Clear(T&& val)
        {
            for (int i = 0; i < MeshPassType::Count; ++i)
            {
                m_data[i] = val;
            }
        }

    private:
        std::array<T, MeshPassType::Count> m_data;
    };

    struct DrawMesh
    {
        bool        isMerged; // if this merged, we get the mesh from scene merged vertexBuffer
        uint32_t    firstVertex;
        uint32_t    firstIndex;
        uint32_t    indexCount;
        uint32_t    vertexCount;
        MeshSource* meshSource;
    };

    struct MeshDrawCommand
    {
        uint64_t          pipelineID;
        DrawMesh          drawMesh;
        Material*         materialProxy;
        VertexFactoryType type;
    };

    struct MeshPass
    {
        using MaterialFilter = std::function<bool(const Material& material)>;

        void Clear();

        MeshPassType                 type;
        std::vector<Ref<StaticMesh>> staticMeshes;
        MaterialFilter               filter;
    };

    MeshPassType Step(MeshPassType type);
} // namespace wind