#pragma once
#include <rf_sim/solver/api/raytracing_config.hpp>
#include <rf_sim/solver/api/solver.hpp>

namespace RfSim::Solver {


    class RaytracingSolver final : public ISolver {
    public:
        explicit RaytracingSolver(const RaytracingConfig &config);

    private:
        std::vector<Model::SignalMap> doSolve(const Model::Scene &scene, SolverObserver &observer) override;

        [[nodiscard]] std::string doName() const override {
            return "Raytracing CPU Solver";
        }

        RaytracingConfig m_config;
    };
}
