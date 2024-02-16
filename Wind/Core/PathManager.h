#pragma once

#include "std.h"

#include "Core/Core.h"

namespace wind
{
    class PathManager
    {
    public:
        NO_COPYABLE(PathManager);

        static PathManager& Get()
        {
            assert(s_instance);
            return *s_instance;
        }

        static void Init();
        static void Quit();

        std::filesystem::path projectPath;
        std::filesystem::path shaderPath;
        std::filesystem::path configPath;
        std::filesystem::path asssetPath;
        std::filesystem::path pipelinePath;

    private:
        PathManager();
        static inline PathManager* s_instance = nullptr;
    };
} // namespace wind