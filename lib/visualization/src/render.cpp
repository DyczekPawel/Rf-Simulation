#include <rf_sim/visualization/render.hpp>
#include <rf_sim/profiling.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace RfSim::Core {
    namespace {
        // Material that gets the signal gradient (first material in materials.json)
        constexpr Model::MaterialId k_backgroundMaterial = 0;

        struct ColorStop {
            float position;
            Model::Rgb color;
        };

        // Turbo-like scale: weak (dark blue) -> strong (dark red)
        constexpr std::array k_colorScale{
            ColorStop{.position = 0.00, .color = {.r = 48, .g = 18, .b = 59}},
            ColorStop{.position = 0.25, .color = {.r = 40, .g = 130, .b = 240}},
            ColorStop{.position = 0.50, .color = {.r = 60, .g = 220, .b = 100}},
            ColorStop{.position = 0.75, .color = {.r = 250, .g = 200, .b = 40}},
            ColorStop{.position = 1.00, .color = {.r = 180, .g = 20, .b = 10}},
        };

        std::uint8_t lerpChannel(const std::uint8_t a, const std::uint8_t b, const float t) {
            const float value = std::lerp(static_cast<float>(a), static_cast<float>(b), t);
            return static_cast<std::uint8_t>(std::clamp(std::lround(value), 0L, 255L));
        }

        /**
         * @param t position on the scale, clamped to [0, 1]
         */
        Model::Rgb colorAt(float t) {
            t = std::clamp(t, 0.0F, 1.0F);
            const auto *const upper = std::ranges::find_if(k_colorScale,
                                                           [t](const ColorStop &s) {
                                                               return s.position >= t;
                                                           });
            if (upper == k_colorScale.begin()) return upper->color;

            const auto &hi = *upper;
            const auto &lo = *std::prev(upper);
            const float local = (t - lo.position) / (hi.position - lo.position);
            return {.r = lerpChannel(lo.color.r, hi.color.r, local),
                    .g = lerpChannel(lo.color.g, hi.color.g, local),
                    .b = lerpChannel(lo.color.b, hi.color.b, local),};
        }
    }

    Model::RgbImage renderSignalMap(const Model::FloorPlan &floorPlan, const Model::SignalMap &signalMap) {
        RF_SIM_PROFILE_FUNCTION();

        const auto width = floorPlan.width();
        const auto height = floorPlan.height();
        if (signalMap.width() != width || signalMap.height() != height)
            throw std::runtime_error("renderSignalMap: signal map and floor plan sizes differ");

        const auto &materialGrid = floorPlan.materialGrid();
        const auto &materials = floorPlan.materials();
        const auto &signal = signalMap.grid();

        auto const isColored = [&](const size_t x, const size_t y) {
            return materialGrid[x, y] == k_backgroundMaterial && std::isfinite(signal[x, y]);
        };

        // Scale range from the cells that will actually be colored
        auto minDbm = std::numeric_limits<float>::infinity();
        auto maxDbm = -std::numeric_limits<float>::infinity();
        {
            RF_SIM_PROFILE_SCOPE("Signal range");
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    if (!isColored(x, y)) continue;
                    minDbm = std::min(minDbm, signal[x, y]);
                    maxDbm = std::max(maxDbm, signal[x, y]);
                }
            }
        }
        const float range = maxDbm - minDbm; // <= 0 (or NaN) when nothing / one value was reached

        Model::RgbImage image{width, height, Model::Rgb{}};
        {
            RF_SIM_PROFILE_SCOPE("Colorize");
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    if (isColored(x, y)) {
                        image[x, y] = colorAt(range > 0.0F ? (signal[x, y] - minDbm) / range : 1.0F);
                    } else {
                        image[x, y] = materials.at(materialGrid[x, y]).rgb;
                    }
                }
            }
        }

        return image;
    }
}
