#pragma once

#include "std.h"

namespace wind
{
    class GPUBuffer;
    class GPUTexture;

    class HandleBase
    {
    public:
        using HandleId                         = uint32_t;
        static constexpr const HandleId nullid = HandleId {std::numeric_limits<HandleId>::max()};

        constexpr HandleBase() noexcept : object(nullid) {}

        // whether this Handle is initialized
        explicit operator bool() const noexcept { return object != nullid; }

        // clear the handle, this doesn't free associated resources
        void clear() noexcept { object = nullid; }

        // compare handles
        bool operator==(const HandleBase& rhs) const noexcept { return object == rhs.object; }
        bool operator!=(const HandleBase& rhs) const noexcept { return object != rhs.object; }
        bool operator<(const HandleBase& rhs) const noexcept { return object < rhs.object; }
        bool operator<=(const HandleBase& rhs) const noexcept { return object <= rhs.object; }
        bool operator>(const HandleBase& rhs) const noexcept { return object > rhs.object; }
        bool operator>=(const HandleBase& rhs) const noexcept { return object >= rhs.object; }

        // get this handle's handleId
        HandleId getId() const noexcept { return object; }

        // initialize a handle, for internal use only.
        explicit HandleBase(HandleId id) noexcept : object(id)
        {
            assert(object != nullid); // usually means an uninitialized handle is used
        }

    protected:
        HandleBase(HandleBase const& rhs) noexcept            = default;
        HandleBase& operator=(HandleBase const& rhs) noexcept = default;

    private:
        HandleId object;
    };

    template<typename T>
    struct Handle : public HandleBase
    {

        Handle() noexcept = default;

        Handle(Handle const& rhs) noexcept = default;

        Handle& operator=(Handle const& rhs) noexcept = default;

        explicit Handle(HandleId id) noexcept : HandleBase(id) {}

        // type-safe Handle cast
        template<typename B, typename = std::enable_if_t<std::is_base_of<T, B>::value>>
        Handle(Handle<B> const& base) noexcept : HandleBase(base)
        {} // NOLINT(hicpp-explicit-conversions,google-explicit-constructor)
    };

    using TextureHandle = Handle<GPUTexture>;
    using BufferHandle  = Handle<GPUBuffer>;
} // namespace wind