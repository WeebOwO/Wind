#pragma once

#include <filesystem>

namespace wind::path
{
    std::filesystem::path GetProjectDir();
    std::filesystem::path GetExecutableDir();
    std::filesystem::path GetShaderRootDir();
    std::filesystem::path GetSPVOutputDir();
} // namespace wind::path