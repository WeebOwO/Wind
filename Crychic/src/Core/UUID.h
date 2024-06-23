#pragma once

#include <cstdint>
#include <vector>

namespace crychic
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID& other);

        operator uint64_t() { return m_UUID; }
        operator const uint64_t() const { return m_UUID; }

    private:
        uint64_t m_UUID;
    };
} // namespace crychic

namespace std
{
    template<>
    struct std::hash<crychic::UUID>
    {
        size_t operator()(const crychic::UUID& uuid) const { return uuid; }
    };
} // namespace std