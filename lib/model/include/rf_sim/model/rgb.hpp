#pragma once
#include "rf_sim/model/grid2d.hpp"

#include <cstdint>

namespace RfSim::Model {
    /**
     * @brief 8-bit RGB color.
     */
    struct Rgb {
        uint8_t r{}, g{}, b{};
        friend bool operator ==(const Rgb &a, const Rgb &b) = default;
    };

    /// Image stored as a grid of RGB pixels.
    using RgbImage = Grid2D<Rgb>;
}
