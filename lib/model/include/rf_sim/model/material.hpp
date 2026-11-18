#pragma once
#include "rf_sim/model/rgb.hpp"

#include <cmath>
#include <cstdint>
#include <string>

namespace RfSim::Model {

    /**
     * @brief Stores materials properties as fields defined in ITU-R P.2040-4.
     *
     * @note Relative permittivity is a * f^b and conductivity is c * f^d, with f in GHz.
     */
    struct ItuMaterialModel {
        float a{}; ///< permittivity coefficient
        float b{}; ///< permittivity frequency exponent
        float c{}; ///< conductivity coefficient
        float d{}; ///< conductivity frequency exponent
    };

    /**
     * @brief Stores material properties.
     */
    struct Material {
        std::string name;          ///< human readable name
        Rgb rgb;                   ///< color representing this material on the floor plan image
        ItuMaterialModel ituModel; ///< electromagnetic properties
    };

    /// Index of a material in \c Materials.
    using MaterialId = uint8_t;
    /// Materials indexed by \c MaterialId, the first one is the default material.
    using Materials = std::vector<Material>;

    /**
     * @brief Stores solved permittivity and conductivity of material depending on frequency.
     */
    struct MaterialEM {
        float epsilonR{}; ///< relative permittivity
        float sigma{};    ///< conductivity in S/m

        MaterialEM() = default;

        /**
         * @brief Construct MaterialEM from ITU model.
         *
         * @param ituModel ITU model of material.
         * @param frequency wave frequency in Hz.
         */
        MaterialEM(const ItuMaterialModel &ituModel, const float frequency) {
            const float frequencyGhz = frequency * 1e-9F;
            epsilonR = ituModel.a * std::pow(frequencyGhz, ituModel.b);
            sigma = ituModel.c * std::pow(frequencyGhz, ituModel.d);
        }
    };
}
