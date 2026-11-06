#include <rf_sim/app/cli_progress.hpp>
#include <rf_sim/profiling.hpp>

#include <iostream>

#include <indicators/cursor_control.hpp>

namespace RfSim::App {
    namespace opt = indicators::option;

    CliProgress::CliProgress() {
        indicators::show_console_cursor(false);
    }

    CliProgress::~CliProgress() {
        if (m_bar && !m_bar->is_completed()) {
            std::cout << '\n'; // solve() was interrupted; don't leave the error message on the bar line
        }
        indicators::show_console_cursor(true);
    }

    void CliProgress::onTransmitterStart(const size_t txIdx, size_t txTotal) {
        m_bar.emplace(
            opt::BarWidth{40},
            opt::Start{"["},
            opt::Fill{"="},
            opt::Lead{">"},
            opt::Remainder{" "},
            opt::End{"]"},
            opt::PrefixText{std::format("TX {}/{} ", txIdx + 1, txTotal)},
            opt::ShowPercentage{true},
            opt::ShowElapsedTime{true},
            opt::ShowRemainingTime{true});
        m_lastPercent = 0;
        m_lastRedraw = {};
    }

    void CliProgress::onProgress(const Solver::ProgressEvent &event) {
        if (!m_bar || event.total == 0) {
            return;
        }
        const size_t percent = std::min<size_t>(event.done * 100 / event.total, 100);
        const auto now = Clock::now();
        if (percent == m_lastPercent && now - m_lastRedraw < k_redrawInterval) {
            return;
        }
        m_lastPercent = percent;
        m_lastRedraw = now;

        RF_SIM_PROFILE_SCOPE("CliProgress redraw");
        m_bar->set_progress(percent);
    }

    void CliProgress::onTransmitterFinish(size_t /*txIdx*/, std::span<const Solver::Metric> /*metrics*/) {
        if (m_bar && !m_bar->is_completed()) {
            m_bar->set_progress(100); // reaching max completes the bar and ends the line
        }
        m_bar.reset();
    }
}
