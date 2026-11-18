#include <rf_sim/app/cli_progress.hpp>
#include <rf_sim/app/logging_session.hpp>
#include <rf_sim/core/simulation.hpp>
#include <rf_sim/profiling.hpp>
#include <rf_sim/profiling_opencv.hpp>

#include <cstdlib>
#include <iostream>

#include <spdlog/spdlog.h>

using namespace RfSim;

int main(const int argc, char *const*argv) {
    RF_SIM_PROFILE_TRACK_OPENCV();
    RF_SIM_PROFILE_THREAD_NAME("main");

    if (argc != 2) {
        std::cerr << "usage: rf_sim <scenario_dir>\n";
        return 2;
    }

    const std::filesystem::path scenarioDir{argv[1]};
    const App::LoggingSession logging(scenarioDir / "rf_sim.log");

    const auto startTime = std::chrono::steady_clock::now();

    try {
        RF_SIM_PROFILE_SCOPE("Scenario");

        std::cout << "Initializing..." << '\n';
        SPDLOG_INFO("Scenario: {}", scenarioDir.string());
        const auto config = Core::loadConfig(argv[1]);
        App::CliProgress progress;
        (void)Core::runSimulation(config, progress);
    } catch (const std::exception &e) {
        SPDLOG_CRITICAL("{}", e.what());
        return EXIT_FAILURE;
    }

    const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
    SPDLOG_INFO("Scenario finished, total duration: {:.3f} s ", seconds);

    std::cout << '\n' << "Press enter to exit..." << '\n';
    std::cin.get();

    return EXIT_SUCCESS;
}
