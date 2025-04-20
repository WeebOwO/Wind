#include "Path.h"

#include "Config.h"

namespace wind
{
    PathManager::PathManager(std::filesystem::path projectDir) : m_ProjectDir(projectDir)
    {
        
    }

    std::filesystem::path PathManager::GetProjectDir() const
    {
        return m_ProjectDir;
    }

    std::filesystem::path PathManager::GetShaderRootDir() const
    {
        return m_ProjectDir / "shaders";
    }
} // namespace wind