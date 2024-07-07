#pragma once

#include <filesystem>
#include <string>

namespace crychic
{
    class Paths
    {
    public:
        static std::filesystem::path MainDir();
        static std::filesystem::path ShaderDir();
    };
} // namespace crychic