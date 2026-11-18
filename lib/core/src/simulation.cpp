#include "context.hpp"
#include "logging_observer.hpp"

#include <rf_sim/solver/api/composite_observer.hpp>
#include <rf_sim/visualization/render.hpp>
#include <rf_sim/core/simulation.hpp>
#include <rf_sim/io/image_io.hpp>
#include <rf_sim/io/materials_io.hpp>
#include <rf_sim/io/transmitters_io.hpp>
#include <rf_sim/profiling.hpp>

#include <fmt/format.h>

namespace RfSim::Core {

    Model::Scene loadScene(const SimulationConfig &config) {
        RF_SIM_PROFILE_FUNCTION();

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
        RF_SIM_PROFILE_FUNCTION();

        const auto write = [&] {
            std::filesystem::create_directories(outputDirectory);

            for (size_t i = 0; i < result.signalMaps.size(); ++i) {
                const auto image = renderSignalMap(result.scene.floorPlan(), result.signalMaps[i]);
                Io::saveImage(outputDirectory / fmt::format("signal_tx{}.png", i), image);
            }
        };

        Detail::withContext("writeResults: " + outputDirectory.string(), write);
    }

    SimulationResult runSimulation(const SimulationConfig &config, Solver::SolverObserver &observer) {
        RF_SIM_PROFILE_FUNCTION();

        const auto run = [&] {
            LoggingObserver logging;
            Solver::CompositeObserver observers{logging, observer};

            RF_SIM_PROFILE_MESSAGE("Loading scene");
            fmt::println("  - Loading scene");
            auto scene = loadScene(config);

            RF_SIM_PROFILE_MESSAGE("Creating solver");
            fmt::println("  - Creating solver");
            const auto solver = Solver::createSolver(config.solver);

            const auto solve = [&] {
                return solver->solve(scene, observers);
            };

            RF_SIM_PROFILE_MESSAGE("Running simulation");
            fmt::println("\nRunning simulation...");
            auto signalMaps = Detail::withContext("solver '" + solver->name() + "'", solve);

            SimulationResult result{.scene = std::move(scene), .signalMaps = std::move(signalMaps)};

            RF_SIM_PROFILE_MESSAGE("Writing results");
            fmt::println("Writing results");
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
