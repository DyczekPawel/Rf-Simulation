#include <rf_sim/solver/solver_factory.hpp>
#include <rf_sim/solver/raytracing_cpu/raytracing_solver.hpp>

namespace RfSim::Solver {
    namespace {
        template <class... Ts>
        struct Overloaded : Ts... {
            using Ts::operator()...;
        };
    }

    std::unique_ptr<ISolver> createSolver(const SolverConfig &config) {
        return std::visit(Overloaded{
                              [](const RaytracingConfig &c) -> std::unique_ptr<ISolver> {
                                  return std::make_unique<RaytracingSolver>(c);
                              },
                          },
                          config);
    }
}
