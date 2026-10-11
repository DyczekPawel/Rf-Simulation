#include <rf_sim/io/image_io.hpp>

#include <common/test_utils.hpp>

#include <gtest/gtest.h>
#include <opencv2/imgcodecs.hpp>

namespace {
    using RfSim::Io::loadImage;
    using RfSim::Model::Rgb;
    using RfSim::Test::TempDir;

    class LoadImageTest : public ::testing::Test {
    protected:
        [[nodiscard]] std::filesystem::path writePng(const cv::Mat &mat) const {
            auto path = m_dir.file("image.png");
            if (!cv::imwrite(path.string(), mat)) throw std::runtime_error("cannot write " + path.string());
            return path;
        }

        TempDir m_dir;
    };

    TEST_F(LoadImageTest, LoadsPixelsAsRgb) {
        // Non-square image with distinct channels catches swapped width/height and BGR/RGB.
        cv::Mat mat(2, 3, CV_8UC3, cv::Scalar(0, 0, 0)); // 2 rows (height) x 3 cols (width), BGR
        mat.at<cv::Vec3b>(0, 0) = {30, 20, 10};
        mat.at<cv::Vec3b>(1, 2) = {255, 0, 0};

        const auto image = loadImage(writePng(mat));

        ASSERT_EQ(image.width(), 3U);
        ASSERT_EQ(image.height(), 2U);
        EXPECT_EQ((image[0, 0]), (Rgb{.r = 10, .g = 20, .b = 30}));
        EXPECT_EQ((image[2, 1]), (Rgb{.r = 0, .g = 0, .b = 255}));
        EXPECT_EQ((image[1, 0]), (Rgb{.r = 0, .g = 0, .b = 0}));
    }

    TEST_F(LoadImageTest, NormalizesGrayscaleAndAlphaToRgb) {
        // Floor plans may be exported as grayscale or RGBA, both must load as plain RGB.
        {
            SCOPED_TRACE("grayscale");
            const auto image = loadImage(writePng(cv::Mat(1, 1, CV_8UC1, cv::Scalar(128))));
            EXPECT_EQ((image[0, 0]), (Rgb{.r = 128, .g = 128, .b = 128}));
        }
        {
            SCOPED_TRACE("transparent RGBA");
            const auto image = loadImage(writePng(cv::Mat(1, 1, CV_8UC4, cv::Scalar(30, 20, 10, 0))));
            EXPECT_EQ((image[0, 0]), (Rgb{.r = 10, .g = 20, .b = 30}));
        }
    }

    TEST_F(LoadImageTest, ThrowsOnUnreadableInput) {
        EXPECT_THROW((void) loadImage(m_dir.file("missing.png")), std::runtime_error);
        EXPECT_THROW((void) loadImage(m_dir.path()), std::runtime_error);
        EXPECT_THROW((void) loadImage(m_dir.writeFile("empty.png", "")), std::runtime_error);
        EXPECT_THROW((void) loadImage(m_dir.writeFile("text.png", "not an image")), std::runtime_error);
    }
}
