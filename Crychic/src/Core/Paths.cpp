#include "Paths.h"

#include "Config.h"
#include "Macro.h"

namespace crychic
{
    static std::string mainDir   = STRINGIFY(MAIN_DIR);
    static std::string shaderDir = STRINGIFY(SHADER_DIR);

    std::filesystem::path Paths::MainDir() { return mainDir; }
    std::filesystem::path Paths::ShaderDir() { return shaderDir; }
} // namespace crychic