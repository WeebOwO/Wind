#pragma once

#include "Singleton.h"

#include <spdlog/spdlog.h>

namespace crychic
{
    class LogManager 
    {
    public:
        enum class Level
        {
            Trace,
            Debug,
            Info,
            Warn,
            Error,
            Critical,
        };

        template<typename... Args>
        void Log(Level level, Args&&... args)
        {
            switch (level)
            {
            case Level::Trace:
                spdlog::trace(std::forward<Args>(args)...);
                break;
            case Level::Debug:
                spdlog::debug(std::forward<Args>(args)...);
                break;
            case Level::Info:
                spdlog::info(std::forward<Args>(args)...);
                break;
            case Level::Warn:
                spdlog::warn(std::forward<Args>(args)...);
                break;
            case Level::Error:
                spdlog::error(std::forward<Args>(args)...);
                break;
            case Level::Critical:
                spdlog::critical(std::forward<Args>(args)...);
                break;
            }
        }
    };

} // namespace wind