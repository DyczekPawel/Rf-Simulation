#pragma once
#include <rf_sim/model/floor_plan.hpp>
#include <rf_sim/model/transmitter.hpp>

namespace RfSim::Model {
    class Scene {
    public:
        Scene(FloorPlan &&floorPlan, std::vector<Transmitter> &&transmitters);

        [[nodiscard]] const FloorPlan &floorPlan() const noexcept {
            return m_floorPlan;
        }

        [[nodiscard]] const std::vector<Transmitter> &transmitters() const noexcept {
            return m_transmitters;
        }

    private:
        FloorPlan m_floorPlan;
        std::vector<Transmitter> m_transmitters;
    };
}
