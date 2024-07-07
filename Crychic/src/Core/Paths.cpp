#include "Paths.h"

#include "Config.h"

namespace crychic
{
    std::filesystem::path Paths::MainDir() { return R"(D:\Code\Wind\Crychic)"; }
    std::filesystem::path Paths::ShaderDir() { return MainDir() / "shaders/"; }
} // namespace crychic::paths