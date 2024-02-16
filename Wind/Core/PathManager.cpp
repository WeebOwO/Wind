#include "PathManager.h"

#include "Core/Config.h"

namespace wind
{
    PathManager::PathManager()
    {
        const std::string projectDir = PROJECT_DIR;
        projectPath                  = PROJECT_DIR;
        shaderPath                   = projectDir + "\\Shaders";
        asssetPath                   = projectDir + "\\Assets";
        pipelinePath                 = asssetPath.string() + "\\RenderPipeline";
    }

    void PathManager::Init()
    {
        assert(!s_instance);
        s_instance = new PathManager();
    }

    void PathManager::Quit()
    {
        assert(s_instance);
        delete s_instance;
        s_instance = nullptr;
    }
} // namespace wind