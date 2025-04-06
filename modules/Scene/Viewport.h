#pragma once

#include <cstdint>

namespace wind
{
    /**
     * Defines a viewport, which is the origin and extent of the clip-space.
     * All drawing is clipped to the viewport.
     */
    struct IViewport
    {
        int32_t  left;   //!< left coordinate in window space.
        int32_t  bottom; //!< bottom coordinate in window space.
        uint32_t width;  //!< width in pixels
        uint32_t height; //!< height in pixels
        //! get the right coordinate in window space of the viewport
        int32_t right() const noexcept { return left + int32_t(width); }
        //! get the top coordinate in window space of the viewport
        int32_t top() const noexcept { return bottom + int32_t(height); }
    };

    class Viewport : public IViewport
    {
    public:
        Viewport(int32_t left, int32_t bottom, uint32_t width, uint32_t height) :
            IViewport {left, bottom, width, height}
        {}

        Viewport()                = default;
        ~Viewport()               = default;
        Viewport(const Viewport&) = default;

        bool Empty() const noexcept { return width == 0 || height == 0; }

    private:
        friend bool operator==(Viewport const& lhs, Viewport const& rhs) noexcept
        {
            return (&rhs == &lhs) || (rhs.left == lhs.left && rhs.bottom == lhs.bottom && rhs.width == lhs.width &&
                                      rhs.height == lhs.height);
        }

        friend bool operator!=(Viewport const& lhs, Viewport const& rhs) noexcept { return !(rhs == lhs); }
    };
} // namespace wind