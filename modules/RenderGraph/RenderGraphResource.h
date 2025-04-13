#pragma once

#include "Backend/Guard.h"
#include <cstdint>

namespace wind
{
    using RDGResourceFlag = uint32_t;

    enum RDGResourceType
    {
        Unknown = 0,
        Buffer,
        Image1D,
        Image2D,
        Image3D,
        TypeCount
    };

    enum RDGResourceFlagBits
    {
        RPS_RESOURCE_FLAG_NONE                   = 0,        ///< No special properties.
        RPS_RESOURCE_FLAG_CUBEMAP_COMPATIBLE_BIT = (1 << 1), ///< Supports cubemap views.
        RPS_RESOURCE_FLAG_ROWMAJOR_IMAGE_BIT     = (1 << 2), ///< Uses rowmajor image layout.
        RPS_RESOURCE_FLAG_PREFER_GPU_LOCAL_CPU_VISIBLE_BIT =
            (1 << 3),                                                 ///< Preferred to be in GPU-local CPU-visible heap
                                                                      ///  if available.
        RPS_RESOURCE_FLAG_PREFER_DEDICATED_ALLOCATION_BIT = (1 << 4), ///< Preferred to be in dedicated allocation or as
                                                                      ///  committed resource.
        RPS_RESOURCE_FLAG_PERSISTENT_BIT = (1 << 15),                 ///< Resource data is persistent from frame to
                                                                      ///  frame.
    };

    struct RDGResourceDesc
    {
        static bool IsBuffer(const RDGResourceType type) { return type == RDGResourceType::Buffer; }

        static bool IsImage(const RDGResourceType type)
        {
            return type == RDGResourceType::Image1D || type == RDGResourceType::Image2D ||
                   type == RDGResourceType::Image3D;
        }

        bool IsBuffer() const { return IsBuffer(Type); }
        bool IsImage() const { return IsImage(Type); }

        static RDGResourceDesc Buffer(uint32_t sizeInBytesLo, uint32_t sizeInBytesHi = 1, RDGResourceFlag flags = 0x0)
        {
            RDGResourceDesc desc;
            desc.Type                 = RDGResourceType::Buffer;
            desc.buffer.sizeInBypesLo = sizeInBytesLo;
            desc.buffer.sizeInBypesHi = sizeInBytesHi;
            desc.flags                = flags;
            return desc;
        }

        static RDGResourceDesc Image1D(vk::Format      format,
                                       uint32_t        width,
                                       uint32_t        mipLevels        = 1,
                                       uint32_t        inArrayLayers    = 1,
                                       uint32_t        sampleCount      = 1,
                                       uint32_t        inTemporalLayers = 1,
                                       RDGResourceFlag flags            = RDGResourceFlagBits::RPS_RESOURCE_FLAG_NONE)
        {
            RDGResourceDesc desc;
            desc.Type              = RDGResourceType::Image1D;
            desc.temporalLayers    = inTemporalLayers;
            desc.image.width       = width;
            desc.image.height      = 1;
            desc.image.depth       = 1;
            desc.image.mipLevels   = mipLevels;
            desc.image.arrayLayers = inArrayLayers;
            desc.image.format      = format;
            desc.image.sampleCount = sampleCount;
            desc.flags             = flags;
            return desc;
        }

        static RDGResourceDesc Image2D(vk::Format      inFormat,
                                       uint32_t        inWidth,
                                       uint32_t        inHeight,
                                       uint32_t        inArrayLayers    = 1,
                                       uint32_t        inMipLevels      = 1,
                                       uint32_t        inSampleCount    = 1,
                                       uint32_t        inTemporalLayers = 1,
                                       RDGResourceFlag flags            = RDGResourceFlagBits::RPS_RESOURCE_FLAG_NONE)
        {
            return RDGResourceDesc {RDGResourceType::Image2D,
                                    inFormat,
                                    inWidth,
                                    inHeight,
                                    inArrayLayers,
                                    inMipLevels,
                                    inSampleCount,
                                    inTemporalLayers,
                                    flags};
        }

        static RDGResourceDesc Image3D(vk::Format      inFormat,
                                       uint32_t        inWidth,
                                       uint32_t        inHeight,
                                       uint32_t        inDepth,
                                       uint32_t        inMipLevels      = 1,
                                       uint32_t        inSampleCount    = 1,
                                       uint32_t        inTemporalLayers = 1,
                                       RDGResourceFlag flags            = RDGResourceFlagBits::RPS_RESOURCE_FLAG_NONE)
        {
            return RDGResourceDesc {RDGResourceType::Image3D,
                                    inFormat,
                                    inWidth,
                                    inHeight,
                                    inDepth,
                                    inMipLevels,
                                    inSampleCount,
                                    inTemporalLayers,
                                    flags};
        }

        // default copy constructor and assignment operator
        RDGResourceDesc(const RDGResourceDesc&)            = default;
        RDGResourceDesc& operator=(const RDGResourceDesc&) = default;

        RDGResourceDesc() : RDGResourceDesc(RDGResourceType::Unknown, vk::Format::eUndefined, 0, 0) {}

        RDGResourceDesc(RDGResourceType type,
                        vk::Format      format,
                        uint32_t        width,
                        uint32_t        height           = 1,
                        uint32_t        depthOrArryLayer = 1,
                        uint32_t        mipLevels        = 1,
                        uint32_t        sampleCount      = 1,
                        uint32_t        inTemporalLayers = 1,
                        RDGResourceFlag flags            = RDGResourceFlagBits::RPS_RESOURCE_FLAG_NONE) : Type(type)
        {
            temporalLayers    = inTemporalLayers;
            image.width       = width;
            image.height      = height;
            image.depth       = depthOrArryLayer;
            image.mipLevels   = mipLevels;
            image.arrayLayers = depthOrArryLayer;
            image.format      = format;
            image.sampleCount = sampleCount;
            this->flags       = flags;
        }

        RDGResourceType Type = RDGResourceType::Unknown;
        union
        {
            struct
            {
                uint32_t width;
                uint32_t height;
                union
                {
                    uint32_t depth;
                    uint32_t arrayLayers;
                };
                uint32_t   mipLevels;
                vk::Format format;
                uint32_t   sampleCount;
            } image;

            struct
            {
                uint32_t sizeInBypesLo;
                uint32_t sizeInBypesHi;
            } buffer;
        };

        uint32_t        temporalLayers = 1;
        RDGResourceFlag flags          = RDGResourceFlagBits::RPS_RESOURCE_FLAG_NONE;
    };

    struct RDGSubresourceRange
    {
        RDGSubresourceRange(uint16_t inBaseMip        = 0,
                            uint16_t inMipLevels      = 1,
                            uint32_t inBaseArrayLayer = 0,
                            uint32_t inArrayLayers    = 1)
        {
            baseMipLevel   = inBaseMip;
            miplevels      = inMipLevels;
            baseArrayLayer = inBaseArrayLayer;
            arrayLayers    = inArrayLayers;
        } 

        uint16_t baseMipLevel;
        uint16_t miplevels;
        uint32_t baseArrayLayer;
        uint32_t arrayLayers;
    };
} // namespace wind