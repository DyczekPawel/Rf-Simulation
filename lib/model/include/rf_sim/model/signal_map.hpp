#pragma once
#include <rf_sim/model/grid2d.hpp>

namespace RfSim::Model {
    class SignalMap {
    public:
        SignalMap(const std::size_t width, const std::size_t height, const float pixelsPerMeter)
            : m_pixelsPerMeter(pixelsPerMeter) {
            m_dbm = Grid2D(width, height, -std::numeric_limits<float>::infinity());
        }

        [[nodiscard]] std::size_t width() const noexcept {
            return m_dbm.width();
        }

        [[nodiscard]] std::size_t height() const noexcept {
            return m_dbm.height();
        }

        [[nodiscard]] float pixelsPerMeter() const noexcept {
            return m_pixelsPerMeter;
        }

        [[nodiscard]] Grid2D<float> &grid() noexcept {
            return m_dbm;
        }

        [[nodiscard]] const Grid2D<float> &grid() const noexcept {
            return m_dbm;
        }

    private:
        Grid2D<float> m_dbm;
        float m_pixelsPerMeter;
    };
}
