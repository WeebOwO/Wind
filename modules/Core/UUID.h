#pragma once

#include <cstdint>

namespace wind
{
    class UUID
    {
    public:
        UUID();

        [[nodiscard]] uint64_t Get() const { return m_uuid; }

        void GenerateUUID();

        operator uint64_t() const { return m_uuid; }

    private:
        uint64_t m_uuid = 0;
    };

    inline bool operator==(const UUID& lhs, const UUID& rhs) { return lhs.Get() == rhs.Get(); }

    inline bool operator!=(const UUID& lhs, const UUID& rhs) { return !(lhs == rhs); }

    inline bool operator<(const UUID& lhs, const UUID& rhs) { return lhs.Get() < rhs.Get(); }

    inline bool operator>(const UUID& lhs, const UUID& rhs) { return lhs.Get() > rhs.Get(); }

    inline bool operator<=(const UUID& lhs, const UUID& rhs) { return lhs.Get() <= rhs.Get(); }

    inline bool operator>=(const UUID& lhs, const UUID& rhs) { return lhs.Get() >= rhs.Get(); }

    inline bool operator==(const UUID& lhs, uint64_t rhs) { return lhs.Get() == rhs; }

    inline bool operator!=(const UUID& lhs, uint64_t rhs) { return !(lhs == rhs); }
}; // namespace wind