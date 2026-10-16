#pragma once
#include <rf_sim/model/floor_plan.hpp>
#include <rf_sim/model/rgb.hpp>
#include <rf_sim/model/signal_map.hpp>

namespace RfSim::Core {
    /**
     * @brief Renders a signal map over the floor plan.
     *
     * @details Cells of the default material (id 0, usually air) are colored by signal strength,
     *          all other cells keep their material color. The color scale spans the weakest to the
     *          strongest finite value found in default material cells, so it adapts to each map.
     *          Cells the signal never reached (-inf) keep the default material color.
     *
     * @throws std::runtime_error if the signal map and floor plan sizes differ.
     */
    [[nodiscard]] Model::RgbImage renderSignalMap(const Model::FloorPlan &floorPlan, const Model::SignalMap &signalMap);
}
