#pragma once
#include <rf_sim/solver/api/raytracing_config.hpp>

#include <filesystem>
#include <optional>
#include <string>

namespace RfSim::Io {
    /**
     * @brief Floor plan input description.
     */
    struct InputConfig {
        std::filesystem::path floorPlan;
        double pixelsPerMeter{0};
    };

    /**
     * @brief Where simulation results are written.
     */
    struct OutputConfig {
        std::filesystem::path directory;
    };

    /**
     * @brief Solver-independent part of the simulation config file.
     *
     * @details Paths are returned exactly as written in the file (possibly relative);
     *          resolving them against the config file location is the caller's job.
     *          Solver parameters are loaded separately, by the loader matching solverType.
     */
    struct SimulationFile {
        InputConfig input;
        OutputConfig output;
        std::string solverType;

        // Optional overrides of the scenario directory convention (e.g. a shared materials library)
        std::optional<std::filesystem::path> materials;
        std::optional<std::filesystem::path> transmitters;
    };

    /**
     * @brief Loads the simulation config file.
     *
     * @details Expected format:
     *          @code{.json}
     *              {
     *                "input": "floor_plan.png",
     *                "output": "./out/",
     *                "pixels_per_meter": 100,
     *                "materials": "../common/materials.json",   // optional
     *                "transmitters": "transmitters.json",       // optional
     *                "solver": { "type": "raytracing_cpu", "params": { ... } }
     *              }
     *          @endcode
     *          Unknown keys are rejected, so typos do not pass silently.
     *
     * @throws std::runtime_error if the file cannot be read or any field is missing or invalid.
     */
    [[nodiscard]] SimulationFile loadSimulationFile(const std::filesystem::path &path);

    /**
     * @brief Loads "solver.params" of the simulation config file as raytracing parameters.
     *
     * @details Every parameter is optional; a missing one (or the whole "params" object)
     *          keeps the RaytracingConfig default. Does not check "solver.type".
     *          Supported keys: "max_bounces", "min_power_dbm", "polarization" ("TE" | "TM").
     *
     * @throws std::runtime_error if the file cannot be read or a present parameter is invalid.
     */
    [[nodiscard]] Solver::RaytracingConfig loadRaytracingConfig(const std::filesystem::path &path);
}
