#include <rf_sim/solver/api/solver.hpp>
#include <rf_sim/solver/api/solver_error.hpp>

namespace RfSim::Solver {
    std::vector<Model::SignalMap> ISolver::solve(const Model::Scene &scene, SolverObserver &observer) {
        const auto &fp = scene.floorPlan();
        const auto results = doSolve(scene, observer);

        if (results.size() != scene.transmitters().size()) {
            throw SolverError(name() + ": result count does not match transmitter count");
        }

        for (const auto &result: results) {
            if (result.height() != fp.height() ||
                result.width() != fp.width()) {
                throw SolverError(name() + ": result size does not match floor plan");
            }
            if (result.pixelsPerMeter() != fp.pixelsPerMeter()) {
                throw SolverError(name() + ": result has different meters per pixel");
            }
        }

        return results;
    }

    std::string ISolver::name() const {
        auto name = doName();

        if (name.empty()) {
            throw SolverError("ISolver: name does not exist");
        }

        return name;
    }
}
