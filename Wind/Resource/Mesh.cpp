#include "Mesh.h"

#include "Backend/Buffer.h"
#include "Backend/Command.h"
#include "Engine/RuntimeContext.h"

namespace wind
{
    void StaticMesh::InitRHI()
    {
        auto& device = g_runtimeContext.device;

        uint32_t vertexBufferByteSize = sizeof(StaticMeshVertexFactory::Vertex) * meshSource.vertices.size();
        uint32_t indexBufferByteSize  = sizeof(StaticMeshVertexFactory::Index) * meshSource.indices.size();

        auto vertexUploadBuffer = device->CreateUploadBuffer(vertexBufferByteSize);
        auto indexUploadBuffer  = device->CreateUploadBuffer(indexBufferByteSize);

        meshSource.vertexBuffer = device->CreateDeviceBuffer(
            vertexBufferByteSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst);
        meshSource.indexBuffer = device->CreateDeviceBuffer(
            indexBufferByteSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);

        vertexUploadBuffer->WriteData(meshSource.vertices.data(), vertexBufferByteSize, 0);
        indexUploadBuffer->WriteData(meshSource.indices.data(), indexBufferByteSize, 0);

        device->ExecuteImmediately([&](vk::CommandBuffer cb) {
            vk::BufferCopy bufferCopy {.srcOffset = 0, .dstOffset = 0, .size = vertexBufferByteSize};
            cb.copyBuffer(vertexUploadBuffer->buffer(), meshSource.vertexBuffer->buffer(), bufferCopy);

            bufferCopy.setSize(indexBufferByteSize);
            cb.copyBuffer(indexUploadBuffer->buffer(), meshSource.indexBuffer->buffer(), bufferCopy);
        });
    }

    void StaticMesh::ReleaseRHI()
    {
        meshSource.vertexBuffer.reset();
        meshSource.indexBuffer.reset();
    }
} // namespace wind