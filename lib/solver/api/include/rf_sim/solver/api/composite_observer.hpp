#pragma once
#include <rf_sim/solver/api/solver.hpp>

#include <functional>
#include <vector>

namespace RfSim::Solver {
    class CompositeObserver final : public SolverObserver {
    public:
        explicit CompositeObserver(const std::initializer_list<std::reference_wrapper<SolverObserver>> observers)
            : m_observers(observers) {
        }

        void onTransmitterStart(const size_t txIdx, const size_t txTotal) override {
            notifyAll(&SolverObserver::onTransmitterStart, txIdx, txTotal);
        }

        void onProgress(const ProgressEvent &event) override {
            notifyAll(&SolverObserver::onProgress, event);
        }

        void onTransmitterFinish(const size_t txIdx, const std::span<const Metric> metrics) override {
            notifyAll(&SolverObserver::onTransmitterFinish, txIdx, metrics);
        }

    private:
        template <typename Method, typename... Args>
        void notifyAll(Method method, const Args &... args) {
            for (const auto &observer: m_observers) {
                std::invoke(method, observer.get(), args...);
            }
        }

        std::vector<std::reference_wrapper<SolverObserver>> m_observers;
    };
}
