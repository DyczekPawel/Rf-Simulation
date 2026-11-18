#pragma once
#include <rf_sim/solver/api/solver.hpp>

#include <chrono>
#include <string>

namespace RfSim::Core {
    /// Writes solver progress and per-transmitter metrics to the log.
    class LoggingObserver final : public Solver::SolverObserver {
    public:
        void onTransmitterStart(size_t txIdx, size_t txTotal) override;
        void onProgress(const Solver::ProgressEvent &event) override;
        void onTransmitterFinish(size_t txIdx, std::span<const Solver::Metric> metrics) override;

    private:
        std::chrono::steady_clock::time_point m_start{};
        size_t m_nextDecile = 1;
    };
}
