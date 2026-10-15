#pragma once
#include <rf_sim/model/scene.hpp>
#include <rf_sim/model/signal_map.hpp>

#include <functional>
#include <memory>

namespace RfSim::Solver {
    struct Metric {
        std::string_view name;
        double value;
    };

    struct ProgressEvent {
        size_t done;
        size_t total;
        std::span<const Metric> metrics;
    };


    class SolverObserver {
    public:
        virtual ~SolverObserver() = default;

        virtual void onTransmitterStart(size_t /*txIdx*/, size_t /*txTotal*/) {
        }

        virtual void onProgress(const ProgressEvent & /*event*/) {
        }

        virtual void onTransmitterFinish(size_t /*txIdx*/, std::span<const Metric> /*metrics*/) {
        }
    };

    class ISolver {
    public:
        virtual ~ISolver() = default;

        std::vector<Model::SignalMap> solve(const Model::Scene &scene, SolverObserver &observer);

        [[nodiscard]] std::string name() const;

    private:
        virtual std::vector<Model::SignalMap> doSolve(const Model::Scene &scene, SolverObserver &observer) = 0;

        [[nodiscard]] virtual std::string doName() const = 0;

    };
}
