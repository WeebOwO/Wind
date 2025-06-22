#include "Descriptor.h"

#include "Core/Log.h"
#include "Device.h"

namespace wind
{
    void DescriptorWriter::WriteImage(uint32_t binding, vk::ImageView imageView, vk::ImageLayout imageLayout, vk::DescriptorType type)
    {
        auto& imageInfo = m_ImageInfos.emplace_back(vk::DescriptorImageInfo {
            .imageView    = imageView,
            .imageLayout  = imageLayout,
        });

        vk::WriteDescriptorSet writeInfo = 
        {
            .dstBinding       = binding,
            .descriptorCount  = 1,
            .descriptorType   = type,
            .pImageInfo       = &imageInfo,
        };

        m_WriteInfos.push_back(writeInfo);
    }

    void DescriptorWriter::WriteBuffer(uint32_t binding, vk::Buffer buffer, vk::DeviceSize offset, vk::DeviceSize range, vk::DescriptorType type)
    {
        auto& bufferInfo = m_BufferInfos.emplace_back(vk::DescriptorBufferInfo {
            .buffer = buffer,
            .offset = offset,
            .range  = range,
        });
        // clang-format off
        vk::WriteDescriptorSet writeInfo = 
        {
            .dstBinding       = binding,
            .descriptorCount  = 1,
            .descriptorType   = type,
            .pBufferInfo      = &bufferInfo,
        };  
        // clang-format on
        m_WriteInfos.push_back(writeInfo);
    }

    void DescriptorWriter::UpdateSet(vk::Device device, vk::DescriptorSet set)
    {
        for (auto& writeInfo : m_WriteInfos)
        {
            writeInfo.dstSet = set;
        }

        device.updateDescriptorSets(static_cast<uint32_t>(m_WriteInfos.size()), m_WriteInfos.data(), 0, nullptr);
    }


    DescriptorAllocator::DescriptorAllocator(Device*                  device,
                                             uint32_t                 maxSets,
                                             std::span<PoolSizeRatio> poolSizeRatios) :
        Resource(device), m_MaxSets(maxSets)
    {
        for (const auto& ratio : poolSizeRatios)
        {
            m_PoolSizeRatios.push_back(ratio);
        }
    }

    vk::DescriptorSet DescriptorAllocator::Allocate(const vk::DescriptorSetLayout& layout) 
    {
        auto pool = GetPool();
        // clang-format off
        vk::DescriptorSetAllocateInfo allocInfo = 
        {
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts      = &layout,
        };
        // clang-format on
        
        m_ReadyPools.push_back(pool);

        auto res = m_Device->GetDevice().allocateDescriptorSets({allocInfo});

        return res[0];
    }

    void DescriptorAllocator::InitRHI()
    {
        auto newPool  = CreatePool(m_MaxSets, m_PoolSizeRatios);
        m_SetsPerPool = m_MaxSets * 1.5f;
        m_ReadyPools.push_back(newPool);
    }

    void DescriptorAllocator::ReleaseRHI() 
    {
        for (auto p : m_ReadyPools) 
        {
            m_Device->GetDevice().destroyDescriptorPool(p);
        }
        m_ReadyPools.clear();
        for (auto p : m_FullPools) 
        {
            m_Device->GetDevice().destroyDescriptorPool(p);
        }
        m_FullPools.clear();
    }

    vk::DescriptorPool DescriptorAllocator::CreatePool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios) 
    {
        std::vector<vk::DescriptorPoolSize> poolSizes;
        for (const auto& ratio : poolRatios) 
        {
            poolSizes.push_back({ratio.type, static_cast<uint32_t>(setCount * ratio.ratio)});
        }
        // clang-format off
        vk::DescriptorPoolCreateInfo poolInfo = 
        {
            .flags        = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets      = setCount,
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes   = poolSizes.data(),
        };
        // clang-format on
        return m_Device->GetDevice().createDescriptorPool(poolInfo);
    }

    vk::DescriptorPool DescriptorAllocator::GetPool()
    {
        vk::DescriptorPool newPool;
        if (m_ReadyPools.size() != 0)
        {
            newPool = m_ReadyPools.back();
            m_ReadyPools.pop_back();
        }
        else
        {
            // need to create a new pool
            newPool = CreatePool(m_SetsPerPool, m_PoolSizeRatios);

            m_SetsPerPool *= 1.5f;
            m_SetsPerPool = std::min(m_SetsPerPool, 4092u);
        }

        return newPool;
    }
} // namespace wind
