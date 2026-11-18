#pragma once
#include <rf_sim/core/simulation_config.hpp>
#include <rf_sim/model/scene.hpp>
#include <rf_sim/model/signal_map.hpp>
#include <rf_sim/solver/api/solver.hpp>

#include <filesystem>
#include <vector>

namespace RfSim::Core {
    struct SimulationResult {
        Model::Scene scene;
        std::vector<Model::SignalMap> signalMaps; // one per transmitter, same order as scene.transmitters()
    };

    /**
     * @brief Reads materials, floor plan image and transmitters, and builds the scene.
     *
     * @throws std::runtime_error if any file cannot be loaded, the floor plan cannot be built
     *          or a transmitter lies outside the floor plan.
     */
    [[nodiscard]] Model::Scene loadScene(const SimulationConfig &config);

    /**
     * @brief Renders every signal map onto the floor plan and saves it as "signal_tx<i>.png".
     *
     * @details Creates outputDirectory if needed; existing files with the same names are overwritten.
     *
     * @throws std::runtime_error if the directory cannot be created or an image cannot be saved.
     */
    void writeResults(const SimulationResult &result, const std::filesystem::path &outputDirectory);

    /**
     * @brief Runs the whole pipeline: loadScene, solve, writeResults.
     *
     * @param observer receives solver progress; it is called only from the calling thread.
     *
     * @throws std::runtime_error from any stage, prefixed with the stage that failed.
     */
    [[nodiscard]] SimulationResult runSimulation(const SimulationConfig &config, Solver::SolverObserver &observer);

    /**
     * @brief Same as above, without progress reporting.
     */
    [[nodiscard]] SimulationResult runSimulation(const SimulationConfig &config);
}
