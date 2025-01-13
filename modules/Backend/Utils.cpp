#include "Utils.h"

#include "Device.h"

namespace wind::backend::utils
{
    BufferRef CreateVertexBuffer(Device* device, const std::vector<StaticMeshVertex>& vertices)
    {
        vk::BufferCreateInfo vertexBufferInfo = {};
        vertexBufferInfo.setSize(sizeof(StaticMeshVertex) * vertices.size());
        vertexBufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
        vertexBufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

        BufferRef buffer = device->CreateResourceRef<Buffer>(vertexBufferInfo, allocInfo);
        buffer->InitRHI();
        buffer->UpdateData(vertices.data(), sizeof(StaticMeshVertex) * vertices.size());
        return buffer;
    }

    BufferRef CreateIndexBuffer(Device* device, const std::vector<uint32_t>& indices)
    {
        vk::BufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.setSize(sizeof(uint32_t) * indices.size());
        indexBufferInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
        indexBufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage                   = VMA_MEMORY_USAGE_CPU_TO_GPU;

        BufferRef buffer = device->CreateResourceRef<Buffer>(indexBufferInfo, allocInfo);
        buffer->InitRHI();
        buffer->UpdateData(indices.data(), sizeof(uint32_t) * indices.size());
        return buffer;
    }
} // namespace wind::backend::utils