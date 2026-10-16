#include "context.hpp"

#include <rf_sim/core/simulation_config.hpp>
#include <rf_sim/io/simulation_config_io.hpp>

#include <string_view>

namespace RfSim::Core {
    namespace {
        // Scenario directory convention
        constexpr std::string_view k_simulationFile = "simulation.json";
        constexpr std::string_view k_materialsFile = "materials.json";
        constexpr std::string_view k_transmittersFile = "transmitters.json";

        // Solver type names used in simulation.json
        constexpr std::string_view k_raytracingCpu = "raytracing_cpu";

        std::filesystem::path resolve(const std::filesystem::path &base, const std::filesystem::path &path) {
            return path.is_absolute() ? path : (base / path).lexically_normal();
        }

        /**
         * @brief Maps the solver type name to the matching SolverConfig alternative and loads its params.
         */
        Solver::SolverConfig makeSolverConfig(const std::string &type, const std::filesystem::path &simulationFile) {
            if (type == k_raytracingCpu) return Io::loadRaytracingConfig(simulationFile);

            throw std::runtime_error("unknown solver type '" + type + "', supported: " + std::string(k_raytracingCpu));
        }
    }

    SimulationConfig loadConfig(const std::filesystem::path &scenarioDir) {
        const auto load = [&] {
            if (!std::filesystem::is_directory(scenarioDir))
                throw std::runtime_error("not a directory");

            const auto simulationFile = scenarioDir / k_simulationFile;
            const auto [input, output, solverType, materials, transmitters] = Io::loadSimulationFile(simulationFile);

            return SimulationConfig{
                .floorPlan = resolve(scenarioDir, input.floorPlan),
                .materials = resolve(scenarioDir, materials.value_or(k_materialsFile)),
                .transmitters = resolve(scenarioDir, transmitters.value_or(k_transmittersFile)),
                .pixelsPerMeter = input.pixelsPerMeter,
                .outputDirectory = resolve(scenarioDir, output.directory),
                .solver = makeSolverConfig(solverType, simulationFile),
            };
        };

        return Detail::withContext("loadConfig: " + scenarioDir.string(), load);
    }
}
