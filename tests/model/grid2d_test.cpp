#include <rf_sim/model/grid2d.hpp>

#include <vector>

#include <gtest/gtest.h>

namespace {
    using RfSim::Model::Grid2D;

    TEST(Grid2DTest, StoresDataRowMajor) {
        // Non-square grid catches swapped x/y or width/height in indexing.
        Grid2D<int> grid(3, 2, std::vector{0, 1, 2, 3, 4, 5});

        ASSERT_EQ(grid.width(), 3U);
        ASSERT_EQ(grid.height(), 2U);
        ASSERT_EQ(grid.size(), 6U);
        EXPECT_EQ((grid[2, 0]), 2);
        EXPECT_EQ((grid[0, 1]), 3);
        EXPECT_EQ(grid.at(2, 1), 5);

        grid[1, 1] = 42;
        EXPECT_EQ(grid.at(1, 1), 42);
    }

    TEST(Grid2DTest, FillsWithValue) {
        const Grid2D<int> grid(2, 3, 7);

        ASSERT_EQ(grid.size(), 6U);
        for (const int value: grid.values())
            EXPECT_EQ(value, 7);
    }

    TEST(Grid2DTest, DefaultConstructedIsEmpty) {
        const Grid2D<int> grid;

        EXPECT_TRUE(grid.empty());
        EXPECT_EQ(grid.width(), 0U);
        EXPECT_EQ(grid.height(), 0U);
    }

    TEST(Grid2DTest, ThrowsOnDataSizeMismatch) {
        EXPECT_THROW((Grid2D<int>(2, 2, std::vector{1, 2, 3})), std::invalid_argument);
    }

    TEST(Grid2DTest, AtThrowsOutOfRange) {
        const Grid2D<int> grid(3, 2);

        EXPECT_TRUE(grid.contains(2, 1));
        EXPECT_FALSE(grid.contains(3, 0));
        EXPECT_FALSE(grid.contains(0, 2));
        EXPECT_THROW((void) grid.at(3, 0), std::out_of_range);
        EXPECT_THROW((void) grid.at(0, 2), std::out_of_range);
    }
}
