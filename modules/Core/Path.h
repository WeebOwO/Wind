#pragma once

#include <filesystem>

namespace wind
{
    class PathManager
    {
    public:
        PathManager() = default;
        PathManager(std::filesystem::path projectDir);

        void MarkRootDir(const std::filesystem::path& rootDir) { m_ProjectDir = rootDir; }

        std::filesystem::path GetProjectDir() const;
        std::filesystem::path GetShaderRootDir() const;

    private:
        std::filesystem::path m_ProjectDir;
        std::filesystem::path m_ShaderRootDir;
    };
} // namespace wind