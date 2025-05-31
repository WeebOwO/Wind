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
        VirtualResource(const RDGResourceDesc& resourceDesc) : desc(resourceDesc), name(""), imported(false) {};
        virtual ~VirtualResource() = default;

        template<typename T>
        T* As()
        {
            return static_cast<T*>(this);
        }   

        void NeedByPass(PassNode* passNode);

        virtual bool IsImage() const { return false; }
        virtual bool IsBuffer() const { return false; }
        virtual vk::Image GetImage() const { return nullptr; }
        virtual vk::Buffer GetBuffer() const { return nullptr; }

        // during dag build pass in render graph
        uint32_t         refCount = 0;
        // first use pass and last use pass
        PassNode*        first    = nullptr;
        PassNode*        last     = nullptr;

        ResourceLifeTime lifeTime = ResourceLifeTime::Transient;
        RDGResourceDesc  desc;
        std::string      name;
        bool             imported;
    };

    struct VirtualImage : public VirtualResource 
    {
        VirtualImage(const RDGResourceDesc& resourceDesc) : VirtualResource(resourceDesc) {};

        bool IsImage() const override { return true; }
        vk::Image GetImage() const override { return image; }

        // non own image and image view
        vk::Image image;
        vk::ImageView imageView;
        vk::ImageLayout imageLayout = vk::ImageLayout::eUndefined;
    };

    struct VirtualBuffer : public VirtualResource
    {
        VirtualBuffer(const RDGResourceDesc& resourceDesc) : VirtualResource(resourceDesc) {};

        bool IsBuffer() const override { return true; }
        vk::Buffer GetBuffer() const override { return buffer; }

        // non own buffer and buffer view
        vk::Buffer     buffer;
        vk::BufferView bufferView;
    };
} // namespace wind