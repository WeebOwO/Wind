#include "Core/UUID.h"

#include <random>

namespace wind
{
    static thread_local std::random_device                      rd;
    static thread_local std::mt19937_64                         gen(rd());
    static thread_local std::uniform_int_distribution<uint64_t> dis;

    UUID::UUID() = default;

    void UUID::GenerateUUID()
    {
        // need to generate unique id
        m_uuid = dis(gen);
    }
} // namespace wind