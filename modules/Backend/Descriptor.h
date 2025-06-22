#pragma once

#include <deque>
#include <vector>

#include "Guard.h"
#include "Resource.h"

namespace wind
{
    class Device;

    class DescriptorWriter
    {
    public:        
        void
        WriteImage(uint32_t binding, vk::ImageView imageView, vk::ImageLayout imageLayout, vk::DescriptorType type);
        void WriteBuffer(uint32_t           binding,
                         vk::Buffer         buffer,
                         vk::DeviceSize     offset,
                         vk::DeviceSize     range,
                         vk::DescriptorType type);

        void UpdateSet(vk::Device device, vk::DescriptorSet set);

    private:
        std::deque<vk::DescriptorImageInfo>  m_ImageInfos;
        std::deque<vk::DescriptorBufferInfo> m_BufferInfos;
        std::vector<vk::WriteDescriptorSet>   m_WriteInfos;
    };

    class DescriptorAllocator : public Resource
    {
    public:
        struct PoolSizeRatio
        {
            vk::DescriptorType type;
            float              ratio;
        };

        DescriptorAllocator(Device* device, uint32_t maxSets, std::span<PoolSizeRatio> poolSizeRatios);

        vk::DescriptorSet Allocate(const vk::DescriptorSetLayout& layout);

        void InitRHI() override;
        void ReleaseRHI() override;

    private:
        vk::DescriptorPool CreatePool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios);
        vk::DescriptorPool GetPool();

        std::vector<PoolSizeRatio>      m_PoolSizeRatios;
        std::vector<vk::DescriptorPool> m_FullPools;
        std::vector<vk::DescriptorPool> m_ReadyPools;
        uint32_t                        m_SetsPerPool;
        uint32_t                        m_MaxSets;
    };
} // namespace wind