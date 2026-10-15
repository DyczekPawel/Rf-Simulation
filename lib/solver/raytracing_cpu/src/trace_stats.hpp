#pragma once
#include "rf_sim/solver/api/solver.hpp"

#include <array>

struct TraceStats {
    size_t raysTraced = 0;
    size_t raysSpawned = 0;
    size_t cellsVisited = 0;

    size_t endedOutOfGrid = 0;
    size_t endedByPower = 0;
    size_t endedByIterLimit = 0; // Should be always 0

    size_t reflectionsSkippedByBounces = 0;
    size_t reflectionsSkippedByPower = 0;

    size_t maxPendingSize = 0;

    [[nodiscard]] std::array<RfSim::Solver::Metric, 9> metrics() const noexcept {
        return {{
            {.name = "rays traced", .value = static_cast<double>(raysTraced)},
            {.name = "rays spawned", .value = static_cast<double>(raysSpawned)},
            {.name = "cells visited", .value = static_cast<double>(cellsVisited)},
            {.name = "ended: out of grid", .value = static_cast<double>(endedOutOfGrid)},
            {.name = "ended: below power", .value = static_cast<double>(endedByPower)},
            {.name = "ended: iteration limit", .value = static_cast<double>(endedByIterLimit)},
            {.name = "reflections skipped: bounces", .value = static_cast<double>(reflectionsSkippedByBounces)},
            {.name = "reflections skipped: power", .value = static_cast<double>(reflectionsSkippedByPower)},
            {.name = "max pending", .value = static_cast<double>(maxPendingSize)},
        }};
    }
};
