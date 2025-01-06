#include "Core/Log.h"

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace wind
{
    void Log::Init()
    {
        std::vector<spdlog::sink_ptr> windowSinks = {std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};
        std::vector<spdlog::sink_ptr> appSinks    = {std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};

        windowSinks[0]->set_pattern("%^[%T] [%l] %n: %v%$");
        appSinks[0]->set_pattern("%^[%T] [%l] %n: %v%$");

        spdlog::init_thread_pool(8192, 1);

        s_CoreLogger = std::make_shared<spdlog::async_logger>("Wind",
                                                              windowSinks.begin(),
                                                              windowSinks.end(),
                                                              spdlog::thread_pool(),
                                                              spdlog::async_overflow_policy::block);
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = std::make_shared<spdlog::async_logger>(
            "APP", appSinks.begin(), appSinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        s_ClientLogger->set_level(spdlog::level::trace);

        spdlog::register_logger(s_CoreLogger);
        spdlog::register_logger(s_ClientLogger);
    }

    void Log::Shutdown()
    {
        s_ClientLogger->flush();
        s_CoreLogger->flush();
        spdlog::drop_all();
        spdlog::shutdown();
    }
} // namespace wind