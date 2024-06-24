#pragma once

#include <filesystem>
#include <string>

namespace crychic::paths
{
    std::filesystem::path MainDir();
    std::filesystem::path ShaderDir();
} // namespace crychic::paths