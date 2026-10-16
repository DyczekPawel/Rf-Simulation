#pragma once
#include <rf_sim/solver/solver_factory.hpp>

#include <filesystem>

namespace RfSim::Core {
    /**
     * @brief Fully resolved description of one simulation run.
     *
     * @details Only describes what to load and how to solve; the files themselves are read by loadScene.
     *          All paths are already resolved against the scenario directory, so the config does not
     *          depend on the process working directory.
     */
    struct SimulationConfig {
        std::filesystem::path floorPlan;
        std::filesystem::path materials;
        std::filesystem::path transmitters;
        double pixelsPerMeter{0};

        std::filesystem::path outputDirectory;

        Solver::SolverConfig solver;
    };

    /**
     * @brief Loads the simulation config from a scenario directory.
     *
     * @details Expected layout (file names are fixed by convention):
     *          @code
     *              <scenarioDir>/
     *                  simulation.json     required
     *                  materials.json      default, can be overridden by "materials" in simulation.json
     *                  transmitters.json   default, can be overridden by "transmitters" in simulation.json
     *          @endcode
     *          Relative paths in simulation.json are resolved against scenarioDir.
     *          Only simulation.json is read here; existence of the other files is checked by loadScene.
     *
     * @throws std::runtime_error if scenarioDir is not a directory, simulation.json is invalid
     *          or the solver type is unknown.
     */
    [[nodiscard]] SimulationConfig loadConfig(const std::filesystem::path &scenarioDir);
}
