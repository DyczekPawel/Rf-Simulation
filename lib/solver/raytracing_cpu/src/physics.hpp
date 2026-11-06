#pragma once
#include "rf_sim/solver/api/polarization.hpp"

#include <numbers>
#include <complex>

namespace RfSim::Solver {
    inline constexpr float k_speedOfLight = 299'792'458.0F;
    inline constexpr float k_neperToDb = 20.0F / static_cast<float>(std::numbers::ln10);

    struct MaterialPhysics {
        std::complex<float> eta;
        float attenuationDbPerM;

        MaterialPhysics(const float epsilonR, const float sigma, const float frequency) {
            const float frequencyGhz = frequency * 1e-9F;
            eta = {epsilonR, -17.98F * sigma / frequencyGhz};

            const float k0 = 2.0F * static_cast<float>(std::numbers::pi) * frequency / k_speedOfLight;
            attenuationDbPerM = k_neperToDb * k0 * std::abs(std::sqrt(eta).imag());
        };
    };

    inline float reflectance(std::complex<float> eta1,
                             std::complex<float> eta2,
                             const float cosTheta,
                             const Polarization pol) noexcept {

        if (eta1.real() > eta2.real()) {
            std::swap(eta1, eta2);
        }

        const float sin2 = std::max(0.0F, 1.0F - (cosTheta * cosTheta));
        const auto n1cos1 = std::sqrt(eta1) * cosTheta;
        const auto n2cos2 = std::sqrt(eta2 - eta1 * sin2);

        std::complex<float> gamma;
        switch (pol) {
        case Polarization::TE:
            gamma = n1cos1 - n2cos2 / n1cos1 + n2cos2;
            break;
        case Polarization::TM:
            gamma = eta2 * n1cos1 - eta1 * n2cos2 / eta2 * n1cos1 + eta1 * n2cos2;
            break;
        }

        return std::clamp(std::norm(gamma), 0.0F, 1.0F);
    }

    inline float fractionToLossDb(const float fraction) noexcept {
        constexpr float k_minFraction = 1e-12F;
        return -10.0F * std::log10(std::max(fraction, k_minFraction));
    }

    inline float fsplDb(const float distance, const float frequency) noexcept {
        return 20.0F * std::log10(4.0F * static_cast<float>(std::numbers::pi) * distance * frequency / k_speedOfLight);
    }

}
