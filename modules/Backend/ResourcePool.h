#pragma once

#include <memory>
#include <vector>

#include "Resource.h"

namespace wind
{

    template<typename T>
    requires std::is_base_of_v<Resource, T> 
    class ResourcePool
    {
    public:
        ResourcePool()  = default;
        ~ResourcePool() = default;

        template<typename... Args>
        Handle<T> Create(Args&&... args)
        {
            uint32_t index;

            if (!m_FreeIndices.empty())
            {
                index = m_FreeIndices.back();
                m_FreeIndices.pop_back();
            }
            else
            {
                index = m_Resources.size();
                m_Resources.push_back(nullptr);
                m_Versions.push_back(0);
            }

            m_Resources[index] = std::make_unique<T>(std::forward<Args>(args)...);
            m_Resources[index]->InitRHI();

            uint32_t version = m_Versions[index];
            return Handle<T>(index, version);
        }

        T* Get(const Handle<T>& handle)
        {
            uint32_t index = handle.index;
            if (index >= m_Resources.size() || m_Versions[index] != handle.version)
            {
                return nullptr;
            }
            return m_Resources[index].get();
        }

        void Free(const Handle<T>& handle)
        {
            uint32_t index = handle.index;
            if (index >= m_Resources.size() || m_Versions[index] != handle.version)
            {
                return;
            }

            m_Resources[index]->ReleaseRHI();
            m_Resources[index].reset();
            m_Versions[index]++;
            m_FreeIndices.push_back(index);
        }

        void Clear()
        {
            for (auto& resource : m_Resources)
            {
                if (resource)
                {
                    resource->ReleaseRHI();
                }
            }

            m_Resources.clear();
        }

    private:
        // make a pool allocator
        std::vector<std::unique_ptr<T>> m_Resources;
        std::vector<uint32_t>           m_Versions;
        std::vector<uint32_t>           m_FreeIndices;
    };
} // namespace wind