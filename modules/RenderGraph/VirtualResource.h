#pragma once

#include "RenderGraphResource.h"

namespace wind
{
    class PassNode;

    enum ResourceLifeTime
    {
        Transient,
        Persistent,
        External,
    };

    class VirtualResource
    {
    public:
        VirtualResource(const RDGResourceDesc& resourceDesc) : desc(resourceDesc) {};
        virtual ~VirtualResource() = default;

        template<typename T>
        T* As()
        {
            return static_cast<T*>(this);
        }   

        void NeedByPass(PassNode* passNode);

        // during dag build pass in render graph
        uint32_t         refCount = 0;
        PassNode*        first    = nullptr;
        PassNode*        last     = nullptr;

        ResourceLifeTime lifeTime = ResourceLifeTime::Transient;
        RDGResourceDesc  desc;
    };

    struct VirtualImage : public VirtualResource 
    {
        VirtualImage(const RDGResourceDesc& resourceDesc) : VirtualResource(resourceDesc) {};

        // non own image and image view
        vk::Image image;
        vk::ImageView imageView;
        vk::ImageLayout imageLayout = vk::ImageLayout::eUndefined;
    };

    struct VirtualBuffer : public VirtualResource
    {
        VirtualBuffer(const RDGResourceDesc& resourceDesc) : VirtualResource(resourceDesc) {};

        // non own buffer and buffer view
        vk::Buffer     buffer;
        vk::BufferView bufferView;
    };
} // namespace wind