#pragma once
#include <stdexcept>

namespace RfSim::Solver {
    class SolverError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
}
