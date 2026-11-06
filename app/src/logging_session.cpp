#include <rf_sim/app/logging_session.hpp>
#include <rf_sim/profiling.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace RfSim::App {
    namespace {
        constexpr size_t k_queueSize = 8192;
        constexpr auto k_flushInterval = std::chrono::seconds(2);

        spdlog::sink_ptr makeConsoleSink() {
            auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            sink->set_level(spdlog::level::err);
            sink->set_pattern("%^[%l]%$ %v");
            return sink;
        }

        spdlog::sink_ptr makeFileSink(const std::filesystem::path &logFile) {
            auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile.string(), true);
            sink->set_level(spdlog::level::trace);
            sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [tid %t] %v");
            return sink;
        }
    }

    LoggingSession::LoggingSession(const std::filesystem::path &logFile) {
        spdlog::init_thread_pool(k_queueSize,
                                 1,
                                 [] {
                                     RF_SIM_PROFILE_THREAD_NAME("spdlog");
                                 });

        std::vector sinks{makeConsoleSink()};
        try {
            sinks.push_back(makeFileSink(logFile));
        } catch (const spdlog::spdlog_ex &e) {
            fmt::println(stderr,
                         "Cannot open log file '{}': {}. Logging errors to console only.",
                         logFile.string(),
                         e.what());
        }

        auto logger = std::make_shared<spdlog::async_logger>(
            "rf_sim",
            sinks.begin(),
            sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::warn);

        spdlog::set_default_logger(std::move(logger));
        spdlog::flush_every(k_flushInterval);
    }

    LoggingSession::~LoggingSession() {
        spdlog::shutdown();
    }
}
