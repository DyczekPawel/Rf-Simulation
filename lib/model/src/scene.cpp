#include <rf_sim/model/scene.hpp>

namespace RfSim::Model {
    Scene::Scene(FloorPlan &&floorPlan, std::vector<Transmitter> &&transmitters)
        : m_floorPlan(std::move(floorPlan)),
          m_transmitters(std::move(transmitters)) {
        for (const auto &transmitter: m_transmitters) {
            const auto [x, y] = transmitter.positionM();

            if (const auto pixelsPerM = m_floorPlan.pixelsPerMeter();
                !m_floorPlan.materialGrid().contains(static_cast<size_t>(x * pixelsPerM),
                                                     static_cast<size_t>(y * pixelsPerM))) {
                throw std::invalid_argument("Scene: transmitter is out-of-bounds");
            }
        }
    }
}
