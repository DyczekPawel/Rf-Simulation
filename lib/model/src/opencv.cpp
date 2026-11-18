#include <rf_sim/model/opencv.hpp>
#include <rf_sim/profiling.hpp>

#include <stdexcept>

cv::Mat RfSim::Model::grid2cv(const RgbImage &img) {
    RF_SIM_PROFILE_FUNCTION();

    if (img.empty()) throw std::invalid_argument("grid2cv: empty image");

    cv::Mat mat(static_cast<int>(img.height()), static_cast<int>(img.width()), CV_8UC3);

    for (std::size_t y = 0; y < img.height(); ++y) {
        auto *row = mat.ptr<cv::Vec3b>(static_cast<int>(y));
        for (std::size_t x = 0; x < img.width(); ++x) {
            const auto &[r, g, b] = img[x, y];
            row[x] = cv::Vec3b(b, g, r); // Convert from RGB to BGR
        }
    }

    return mat;
}

RfSim::Model::RgbImage RfSim::Model::cv2grid(const cv::Mat &mat) {
    RF_SIM_PROFILE_FUNCTION();

    if (mat.empty()) throw std::invalid_argument("cv2grid: empty cv image");
    if (mat.type() != CV_8UC3) throw std::invalid_argument("cv2grid: expected CV_8UC3");

    const auto width = static_cast<size_t>(mat.cols);
    const auto height = static_cast<size_t>(mat.rows);

    std::vector<Rgb> pixels;
    pixels.reserve(width * height);

    for (int y = 0; y < mat.rows; ++y) {
        const auto *row = mat.ptr<cv::Vec3b>(y);
        for (int x = 0; x < mat.cols; ++x) {
            const cv::Vec3b &p = row[x];
            //NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
            pixels.push_back({.r = p[2], .g = p[1], .b = p[0]}); // Convert from BGR to RGB
        }
    }

    return {width, height, std::move(pixels)};
}
