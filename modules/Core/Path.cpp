#include "Path.h"

#include "Config.h"

namespace wind
{
    std::filesystem::path path::GetProjectDir()
    {
        const std::string projectDir = PROJECT_DIR;
        return std::filesystem::path(projectDir);
    }
    
    std::filesystem::path path::GetExecutableDir() { return GetProjectDir().parent_path(); }
    std::filesystem::path path::GetShaderRootDir() { return GetProjectDir() / "shaders"; }
    std::filesystem::path path::GetSPVOutputDir() { return GetProjectDir() / "shaders" / "CompiledSPV"; }
} // namespace wind