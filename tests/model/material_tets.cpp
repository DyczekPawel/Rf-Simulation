#include <rf_sim/model/material.hpp>

#include <gtest/gtest.h>

namespace {
    using RfSim::Model::ItuMaterialModel;
    using RfSim::Model::MaterialEM;

    TEST(MaterialEMTest, ComputesFromItuModel) {
        // Distinct coefficients and non-zero exponents catch swapped fields and a wrong Hz -> GHz conversion.
        constexpr ItuMaterialModel k_ituModel{.a = 2.0, .b = 1.0, .c = 3.0, .d = 2.0};

        const MaterialEM em(k_ituModel, 2e9);

        EXPECT_DOUBLE_EQ(em.epsilonR, 4.0); // 2 * 2^1
        EXPECT_DOUBLE_EQ(em.sigma, 12.0);   // 3 * 2^2
    }

    TEST(MaterialEMTest, ReturnsCoefficientsAtOneGhz) {
        // At 1 GHz the frequency term is 1, so the result equals the ITU coefficients (concrete, ITU-R P.2040).
        constexpr ItuMaterialModel k_concrete{.a = 5.24, .b = 0.0, .c = 0.0462, .d = 0.7822};

        const MaterialEM em(k_concrete, 1e9);

        EXPECT_DOUBLE_EQ(em.epsilonR, 5.24);
        EXPECT_DOUBLE_EQ(em.sigma, 0.0462);
    }
}
