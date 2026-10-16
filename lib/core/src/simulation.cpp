#include "context.hpp"

#include <rf_sim/visualization/render.hpp>
#include <rf_sim/core/simulation.hpp>
#include <rf_sim/io/image_io.hpp>
#include <rf_sim/io/materials_io.hpp>
#include <rf_sim/io/transmitters_io.hpp>

#include <format>

namespace RfSim::Core {

    Model::Scene loadScene(const SimulationConfig &config) {
        const auto load = [&] {
            auto materials = Io::loadMaterials(config.materials);
            const auto image = Io::loadImage(config.floorPlan);
            auto transmitters = Io::loadTransmitters(config.transmitters);

            auto floorPlan = Model::FloorPlan::fromImage(image, std::move(materials), config.pixelsPerMeter);

            return Model::Scene{std::move(floorPlan), std::move(transmitters)};
        };

        return Detail::withContext("loadScene", load);
    }

    void writeResults(const SimulationResult &result, const std::filesystem::path &outputDirectory) {
        const auto write = [&] {
            std::filesystem::create_directories(outputDirectory);

            for (size_t i = 0; i < result.signalMaps.size(); ++i) {
                const auto image = renderSignalMap(result.scene.floorPlan(), result.signalMaps[i]);
                Io::saveImage(outputDirectory / std::format("signal_tx{}.png", i), image);
            }
        };

        Detail::withContext("writeResults: " + outputDirectory.string(), write);
    }

    SimulationResult runSimulation(const SimulationConfig &config, Solver::SolverObserver &observer) {
        const auto run = [&] {
            auto scene = loadScene(config);
            const auto solver = Solver::createSolver(config.solver);

            const auto solve = [&] {
                return solver->solve(scene, observer);
            };
            auto signalMaps = Detail::withContext("solver '" + solver->name() + "'", solve);

            SimulationResult result{.scene = std::move(scene), .signalMaps = std::move(signalMaps)};
            writeResults(result, config.outputDirectory);
            return result;
        };

        return Detail::withContext("runSimulation", run);
    }

    SimulationResult runSimulation(const SimulationConfig &config) {
        Solver::SolverObserver noop;
        return runSimulation(config, noop);
    }
}
