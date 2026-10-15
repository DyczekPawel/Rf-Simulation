#pragma once
#include "rf_sim/solver/api/polarization.hpp"

#include <numbers>
#include <complex>

namespace RfSim::Solver {
    inline constexpr double k_speedOfLight = 299'792'458.0;
    inline constexpr double k_neperToDb = 20.0 / std::numbers::ln10;

    struct MaterialPhysics {
        std::complex<double> eta;
        double attenuationDbPerM;

        MaterialPhysics(const double epsilonR, const double sigma, const double frequency) {
            const double frequencyGhz = frequency * 1e-9;
            eta = {epsilonR, -17.98 * sigma / frequencyGhz};

            const double k0 = 2 * std::numbers::pi * frequency / k_speedOfLight;
            attenuationDbPerM = k_neperToDb * k0 * std::abs(std::sqrt(eta).imag());
        };
    };

    inline double reflectance(std::complex<double> eta1,
                              std::complex<double> eta2,
                              const double cosTheta,
                              const Polarization pol) noexcept {

        if (eta1.real() > eta2.real()) {
            std::swap(eta1, eta2);
        }

        const double sin2 = std::max(0.0, 1.0 - (cosTheta * cosTheta));
        const auto n1cos1 = std::sqrt(eta1) * cosTheta;
        const auto n2cos2 = std::sqrt(eta2 - eta1 * sin2);

        std::complex<double> gamma;
        switch (pol) {
        case Polarization::TE:
            gamma = n1cos1 - n2cos2 / n1cos1 + n2cos2;
            break;
        case Polarization::TM:
            gamma = eta2 * n1cos1 - eta1 * n2cos2 / eta2 * n1cos1 + eta1 * n2cos2;
            break;
        }

        return std::clamp(std::norm(gamma), 0.0, 1.0);
    }

    inline double fractionToLossDb(const double fraction) noexcept {
        constexpr double k_minFraction = 1e-12;
        return -10.0 * std::log10(std::max(fraction, k_minFraction));
    }

    inline double fsplDb(const double distance, const double frequency) noexcept {
        return 20.0 * std::log10(4.0 * std::numbers::pi * distance * frequency / k_speedOfLight);
    }

}
