#pragma once
#include "rf_sim/model/rgb.hpp"

#include <opencv2/core/mat.hpp>

namespace RfSim::Model {
    /**
     * @brief Converts \c RgbImage to OpenCV matrix.
     *
     * @param img non-empty image.
     *
     * @return \c CV_8UC3 matrix in BGR channel order.
     *
     * @throws std::invalid_argument on empty image.
     */
    cv::Mat grid2cv(const RgbImage &img);

    /**
     * @brief Converts OpenCV matrix to \c RgbImage.
     *
     * @param mat non-empty \c CV_8UC3 matrix in BGR channel order.
     *
     * @return Image in RGB channel order.
     *
     * @throws std::invalid_argument on empty matrix.
     * @throws std::invalid_argument if matrix type is not CV_8UC3.
     */
    RgbImage cv2grid(const cv::Mat &mat);
}
