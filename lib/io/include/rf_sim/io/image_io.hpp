#pragma once
#include <rf_sim/model/rgb.hpp>

#include <filesystem>

namespace RfSim::Io {
    /**
     * @brief Loads an image file (any format supported by OpenCV) as RgbImage.
     *
     * @param path path to the image file.
     *
     * @return Decoded image in RGB channel order.
     *
     * @throws std::runtime_error if the file cannot be read, is empty, cannot be decoded or converted.
     */
    Model::RgbImage loadImage(const std::filesystem::path &path);
}
