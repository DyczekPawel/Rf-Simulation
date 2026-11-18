#include "logging_observer.hpp"

#include <rf_sim/profiling.hpp>

#include <iterator>

#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace RfSim::Core {
    namespace {
        std::string formatMetrics(std::span<const Solver::Metric> metrics) {
            std::string out;
            for (const auto &[name, value]: metrics) {
                fmt::format_to(std::back_inserter(out), " | {}: {}", name, value);
            }
            return out;
        }
    }

    void LoggingObserver::onTransmitterStart(const size_t txIdx, size_t txTotal) {
        m_start = std::chrono::steady_clock::now();
        m_nextDecile = 1;
        SPDLOG_INFO("Transmitter {}/{}: started", txIdx + 1, txTotal);
    }

    void LoggingObserver::onProgress(const Solver::ProgressEvent &event) {
        if (event.total == 0) {
            return;
        }
        const size_t decile = event.done * 10 / event.total;
        if (decile < m_nextDecile) {
            return;
        }
        m_nextDecile = decile + 1;

        RF_SIM_PROFILE_SCOPE("Log progress");
        SPDLOG_DEBUG("Progress {}/{} ({}%){}", event.done, event.total, decile * 10, formatMetrics(event.metrics));
    }

    void LoggingObserver::onTransmitterFinish(const size_t txIdx, const std::span<const Solver::Metric> metrics) {
        const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - m_start).count();
        SPDLOG_INFO("Transmitter {}: finished in {:.3f} s{}", txIdx + 1, seconds, formatMetrics(metrics));
    }
}
