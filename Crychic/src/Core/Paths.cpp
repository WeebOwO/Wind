#include "Paths.h"

#include "Config.h"

namespace crychic::paths
{
    std::filesystem::path MainDir() { return R"(D:\Code\Wind\Crychic)"; }
    std::filesystem::path ShaderDir() { return MainDir() / "shaders/"; }
} // namespace crychic::paths