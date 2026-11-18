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

    /**
     * @brief Saves an RgbImage to a file; the format is chosen from the path extension (e.g. ".png").
     *
     * @details The parent directory must already exist; creating it is the caller's decision.
     *
     * @throws std::runtime_error if the extension is missing or unsupported, encoding fails
     *          or the file cannot be written.
     */
    void saveImage(const std::filesystem::path &path, const Model::RgbImage &image);
}
