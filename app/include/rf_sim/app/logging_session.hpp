#pragma once
#include <filesystem>

namespace RfSim::App {
    class LoggingSession {
    public:
        explicit LoggingSession(const std::filesystem::path &logFile);
        ~LoggingSession();

        LoggingSession(const LoggingSession &) = delete;
        LoggingSession &operator=(const LoggingSession &) = delete;

        LoggingSession(const LoggingSession &&) = delete;
        LoggingSession &operator=(const LoggingSession &&) = delete;
    };
}
