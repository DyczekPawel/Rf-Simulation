#pragma once
#include <rf_sim/model/grid2d.hpp>

namespace RfSim::Model {
    class SignalMap {
    public:
        SignalMap(const std::size_t width, const std::size_t height, const double pixelsPerMeter)
            : m_pixelsPerMeter(pixelsPerMeter) {
            m_dbm = Grid2D(width, height, -std::numeric_limits<double>::infinity());
        }

        [[nodiscard]] std::size_t width() const noexcept {
            return m_dbm.width();
        }

        [[nodiscard]] std::size_t height() const noexcept {
            return m_dbm.height();
        }

        [[nodiscard]] double pixelsPerMeter() const noexcept {
            return m_pixelsPerMeter;
        }

        [[nodiscard]] Grid2D<double> &grid() noexcept {
            return m_dbm;
        }

        [[nodiscard]] const Grid2D<double> &grid() const noexcept {
            return m_dbm;
        }

    private:
        Grid2D<double> m_dbm;
        double m_pixelsPerMeter;
    };
}
