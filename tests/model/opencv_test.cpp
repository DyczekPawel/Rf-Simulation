#include <rf_sim/model/opencv.hpp>

#include <common/test_utils.hpp>

#include <gtest/gtest.h>

namespace {
    using RfSim::Model::cv2grid;
    using RfSim::Model::grid2cv;
    using RfSim::Model::Rgb;
    using RfSim::Model::RgbImage;

    TEST(OpenCvConversionTest, ConvertsGridToBgrMat) {
        // Non-square image with distinct channels catches swapped width/height and BGR/RGB.
        RgbImage image(3, 2);
        image[2, 1] = {.r = 10, .g = 20, .b = 30};

        const auto mat = grid2cv(image);

        ASSERT_EQ(mat.cols, 3);
        ASSERT_EQ(mat.rows, 2);
        ASSERT_EQ(mat.type(), CV_8UC3);
        EXPECT_EQ(mat.at<cv::Vec3b>(1, 2), cv::Vec3b(30, 20, 10));
    }

    TEST(OpenCvConversionTest, RoundTripPreservesPixels) {
        RgbImage image(3, 2);
        image[0, 0] = {.r = 10, .g = 20, .b = 30};
        image[2, 1] = {.r = 255, .g = 0, .b = 128};

        const auto result = cv2grid(grid2cv(image));

        ASSERT_EQ(result.width(), image.width());
        ASSERT_EQ(result.height(), image.height());
        for (std::size_t y = 0; y < image.height(); ++y) {
            for (std::size_t x = 0; x < image.width(); ++x) {
                EXPECT_EQ((result[x, y]), (image[x, y])) << "at (" << x << ", " << y << ")";
            }
        }
    }

    TEST(OpenCvConversionTest, ThrowsOnInvalidInput) {
        EXPECT_THROW((void) grid2cv(RgbImage{}), std::invalid_argument);
        EXPECT_THROW((void) cv2grid(cv::Mat{}), std::invalid_argument);
        EXPECT_THROW((void) cv2grid(cv::Mat(1, 1, CV_8UC1)), std::invalid_argument);
    }
}
