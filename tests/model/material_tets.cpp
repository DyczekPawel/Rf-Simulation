#include <rf_sim/model/material.hpp>

#include <gtest/gtest.h>

namespace {
    using RfSim::Model::ItuMaterialModel;
    using RfSim::Model::MaterialEM;

    TEST(MaterialEMTest, ComputesFromItuModel) {
        // Distinct coefficients and non-zero exponents catch swapped fields and a wrong Hz -> GHz conversion.
        constexpr ItuMaterialModel k_ituModel{.a = 2.0F, .b = 1.0F, .c = 3.0F, .d = 2.0F};

        const MaterialEM em(k_ituModel, 2e9F);

        EXPECT_FLOAT_EQ(em.epsilonR, 4.0F); // 2 * 2^1
        EXPECT_FLOAT_EQ(em.sigma, 12.0F);   // 3 * 2^2
    }

    TEST(MaterialEMTest, ReturnsCoefficientsAtOneGhz) {
        // At 1 GHz the frequency term is 1, so the result equals the ITU coefficients (concrete, ITU-R P.2040).
        constexpr ItuMaterialModel k_concrete{.a = 5.24F, .b = 0.0F, .c = 0.0462F, .d = 0.7822F};

        const MaterialEM em(k_concrete, 1e9F);

        EXPECT_FLOAT_EQ(em.epsilonR, 5.24F);
        EXPECT_FLOAT_EQ(em.sigma, 0.0462F);
    }
}
