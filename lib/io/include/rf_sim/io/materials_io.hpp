#pragma once
#include <rf_sim/model/material.hpp>

#include  <filesystem>

namespace RfSim::Io {
    /**
     * @brief Loads materials from a JSON config file.
     *
     * @details File must contain a non-empty array of objects of the form:
     *          @code{.json}
     *              { "name": "concrete", "rgb": [0, 0, 0], "itu": { "a": 5.24, "b": 0.0, "c": 0.0462, "d": 0.7822 } }
     *          @endcode
     *          Order is preserved, so the first material becomes the default one (id 0).
     *
     * @param path path to the JSON file.
     *
     * @return Materials in the order they appear in the file.
     *
     * @throws std::runtime_error if the file cannot be opened, is not valid JSON, is not a non-empty array
     *          or any material object is invalid.
     */
    Model::Materials loadMaterials(const std::filesystem::path &path);
}
