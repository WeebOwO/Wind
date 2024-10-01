#pragma once

#include <format>
#include <spdlog/spdlog.h>
#include <utility>

namespace wind
{
    class Log
    {
    public:
        enum class Type : uint8_t
        {
            Core   = 0,
            Client = 1
        };

        enum class Level : uint8_t
        {
            Trace = 0,
            Info,
            Warn,
            Error,
            Fatal
        };

        struct TagDetails
        {
            bool  Enabled     = true;
            Level LevelFilter = Level::Trace;
        };

        static void Init();
        static void Shutdown();

        static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_coreLogger; }
        static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_clientLogger; }

        template<typename... Args>
        static void
        PrintMessage(Log::Type type, Log::Level level, spdlog::format_string_t<Args...> fmt, Args&&... args);

    private:
        inline static std::shared_ptr<spdlog::logger> s_coreLogger   = nullptr;
        inline static std::shared_ptr<spdlog::logger> s_clientLogger = nullptr;
    };

    template<typename... Args>
    void Log::PrintMessage(Log::Type type, Log::Level level, spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();

        switch (level)
        {
            case Level::Trace:
                logger->trace(fmt, std::forward<Args>(args)...);
                break;
            case Level::Info:
                logger->info(fmt, std::forward<Args>(args)...);
                break;
            case Level::Warn:
                logger->warn(fmt, std::forward<Args>(args)...);
                break;
            case Level::Error:
                logger->error(fmt, std::forward<Args>(args)...);
                break;
            case Level::Fatal:
                logger->critical(fmt, std::forward<Args>(args)...);
                break;
        }
    }

// utils macros for core
#define WIND_CORE_TRACE(...) ::wind::Log::PrintMessage(::wind::Log::Type::Core, ::wind::Log::Level::Trace, __VA_ARGS__)
#define WIND_CORE_INFO(...) ::wind::Log::PrintMessage(::wind::Log::Type::Core, ::wind::Log::Level::Info, __VA_ARGS__)
#define WIND_CORE_WARN(...) ::wind::Log::PrintMessage(::wind::Log::Type::Core, ::wind::Log::Level::Warn, __VA_ARGS__)
#define WIND_CORE_ERROR(...) ::wind::Log::PrintMessage(::wind::Log::Type::Core, ::wind::Log::Level::Error, __VA_ARGS__)
#define WIND_CORE_FATAL(...) ::wind::Log::PrintMessage(::wind::Log::Type::Core, ::wind::Log::Level::Fatal, __VA_ARGS__)

// utils macros for client
#define WIND_CLIENT_TRACE(...) \
    ::wind::Log::PrintMessage(::wind::Log::Type::Client, ::wind::Log::Level::Trace, __VA_ARGS__)
#define WIND_CLIENT_INFO(...) \
    ::wind::Log::PrintMessage(::wind::Log::Type::Client, ::wind::Log::Level::Info, __VA_ARGS__)
#define WIND_CLIENT_WARN(...) \
    ::wind::Log::PrintMessage(::wind::Log::Type::Client, ::wind::Log::Level::Warn, __VA_ARGS__)
#define WIND_CLIENT_ERROR(...) \
    ::wind::Log::PrintMessage(::wind::Log::Type::Client, ::wind::Log::Level::Error, __VA_ARGS__)
#define WIND_CLIENT_FATAL(...) \
    ::wind::Log::PrintMessage(::wind::Log::Type::Client, ::wind::Log::Level::Fatal, __VA_ARGS__)
} // namespace wind