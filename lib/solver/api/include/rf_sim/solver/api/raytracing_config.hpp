#pragma once
#include <rf_sim/solver/api/polarization.hpp>

namespace RfSim::Solver {
    struct RaytracingConfig {
        unsigned maxBounces = 3;
        double minPowerDbm = -90.0;
        Polarization polarization = Polarization::TE;
    };
}
