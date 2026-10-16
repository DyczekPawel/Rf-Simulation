#pragma once
#include <rf_sim/solver/api/solver.hpp>

#include <chrono>
#include <optional>

#include <indicators/progress_bar.hpp>

namespace RfSim::App {
    /// Draws one console progress bar per transmitter.
    class CliProgress final : public Solver::SolverObserver {
    public:
        CliProgress();
        ~CliProgress() override;

        CliProgress(const CliProgress &) = delete;
        CliProgress &operator=(const CliProgress &) = delete;

        void onTransmitterStart(size_t txIdx, size_t txTotal) override;
        void onProgress(const Solver::ProgressEvent &event) override;
        void onTransmitterFinish(size_t txIdx, std::span<const Solver::Metric> metrics) override;

    private:
        using Clock = std::chrono::steady_clock;
        static constexpr auto k_redrawInterval = std::chrono::milliseconds(100);

        std::optional<indicators::ProgressBar> m_bar;
        Clock::time_point m_lastRedraw{};
        size_t m_lastPercent = 0;
    };
}
