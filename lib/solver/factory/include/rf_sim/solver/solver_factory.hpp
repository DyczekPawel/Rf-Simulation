#pragma once
#include <rf_sim/solver/api/solver.hpp>
#include <rf_sim/solver/api/raytracing_config.hpp>

#include <memory>
#include <variant>

namespace RfSim::Solver {
    using SolverConfig = std::variant<RaytracingConfig>;

    [[nodiscard]] std::unique_ptr<ISolver> createSolver(const SolverConfig &config);
}
