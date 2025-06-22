#include "GPUScene.h"

#include "Backend/Device.h"

namespace wind
{
    GPUScene::GPUScene(Device* device) : m_Device(device) {}

    void GPUScene::UploadMesh(Mesh& mesh)
    {
        // generate vertex buffer and index buffer
        for (auto& subMesh : mesh.subMeshes)
        {
            // clang-format off
            BufferDesc vertexBufferDesc = 
            {
                .byteSize    = static_cast<uint32_t>(subMesh.vertices.size() * sizeof(Vertex)),
                .usage       = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                .memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
            };

            BufferDesc indexBufferDesc = 
            {
                .byteSize    = static_cast<uint32_t>(subMesh.indices.size() * sizeof(uint32_t)),
                .usage       = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                .memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
            };
            // clang-format on

            Handle<GPUBuffer> vertexBuffer = m_Device->Create(vertexBufferDesc);
            Handle<GPUBuffer> indexBuffer  = m_Device->Create(indexBufferDesc);

            m_Device->UploadDataToGPU(subMesh.vertices.data(), subMesh.vertices.size() * sizeof(Vertex), vertexBuffer);
            m_Device->UploadDataToGPU(subMesh.indices.data(), subMesh.indices.size() * sizeof(uint32_t), indexBuffer);

            subMesh.vertexBuffer = vertexBuffer;
            subMesh.indexBuffer  = indexBuffer;

            m_Meshes.push_back(mesh);
        }
    }

    void GPUScene::ReleaseScene()
    {
        for (auto& mesh : m_Meshes)
        {
            for (auto& subMesh : mesh.subMeshes)
            {
                m_Device->Free(subMesh.vertexBuffer);
                m_Device->Free(subMesh.indexBuffer);
            }
        }

        m_Meshes.clear();
    }
} // namespace wind