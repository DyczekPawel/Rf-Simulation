#include <rf_sim/model/floor_plan.hpp>

#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {
    using RfSim::Model::FloorPlan;
    using RfSim::Model::Grid2D;
    using RfSim::Model::MaterialId;
    using RfSim::Model::Materials;
    using RfSim::Model::Rgb;
    using RfSim::Model::RgbImage;

    /// Returns \p count materials; colors are distinct for up to 256 materials.
    Materials makeMaterials(const std::size_t count) {
        Materials materials(count);
        for (std::size_t i = 0; i < count; ++i) {
            materials.at(i).name = "material " + std::to_string(i);
            materials.at(i).rgb = {.r = static_cast<uint8_t>(i), .g = 0, .b = 0};
        }
        return materials;
    }

    Grid2D<MaterialId> makeGrid(std::vector<MaterialId> ids) {
        const auto width = ids.size();
        return {width, 1, std::move(ids)};
    }

    TEST(FloorPlanTest, ConstructsFromValidArguments) {
        EXPECT_NO_THROW((void) FloorPlan(makeGrid({0, 1, 1}), makeMaterials(2), 10.0));
    }

    TEST(FloorPlanTest, ThrowsOnInvalidArguments) {
        EXPECT_THROW((void) FloorPlan(makeGrid({0}), makeMaterials(0), 10.0), std::invalid_argument);
        EXPECT_THROW((void) FloorPlan(makeGrid({0}), makeMaterials(257), 10.0), std::invalid_argument);
        EXPECT_THROW((void) FloorPlan(makeGrid({0}), makeMaterials(1), 0.0), std::invalid_argument);
        EXPECT_THROW((void) FloorPlan(makeGrid({0}), makeMaterials(1), -1.0), std::invalid_argument);
        EXPECT_THROW((void) FloorPlan(Grid2D<MaterialId>{}, makeMaterials(1), 10.0), std::invalid_argument);
        EXPECT_THROW((void) FloorPlan(makeGrid({0, 2}), makeMaterials(2), 10.0), std::invalid_argument);
    }

    TEST(FloorPlanTest, FromImageAcceptsUnknownColors) {
        // Pixels with no matching material fall back to the default material instead of failing.
        RgbImage image(2, 1);
        image[0, 0] = makeMaterials(2).at(1).rgb;
        image[1, 0] = {.r = 200, .g = 200, .b = 200};

        EXPECT_NO_THROW((void) FloorPlan::fromImage(image, makeMaterials(2), 10.0));
    }

    TEST(FloorPlanTest, FromImageThrowsOnEmptyMaterials) {
        const RgbImage image(1, 1, Rgb{});

        EXPECT_THROW((void) FloorPlan::fromImage(image, makeMaterials(0), 10.0), std::runtime_error);
    }
}
