#include "rf_sim/core/simulation.hpp"

#include <cstdlib>
#include <iostream>

using namespace RfSim;

namespace {
    class CliProgress : public Solver::SolverObserver {
    public:
        void onTransmitterStart(size_t txIdx, size_t txTotal) override {

        }

        void onProgress(const Solver::ProgressEvent &event) override {

        }

        void onTransmitterFinish(size_t txIdx, std::span<const Solver::Metric> metrics) override {

        }
    };
}

int main(const int argc, char *const*argv) {
    if (argc != 2) {
        std::cerr << "usage: rf_sim <scenario_dir>\n";
        return 2;
    }
    try {
        const auto config = Core::loadConfig(argv[1]);
        CliProgress progress;
        (void)Core::runSimulation(config, progress);
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
