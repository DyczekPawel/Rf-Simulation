#pragma once
#include <rf_sim/model/transmitter.hpp>

#include <filesystem>
#include <vector>

namespace RfSim::Io {
    /**
     * @brief Loads transmitters from a JSON config file.
     *
     * @details File must contain a non-empty array of objects of the form:
     *          @code{.json}
     *              {
     *                "name": "Transmitter 1",
     *                "properties": { "frequency": 2.4e9, "dBm": 20 },
     *                "position": { "x": 0, "y": 0 }
     *              }
     *          @endcode
     *          Frequency in Hz, power in dBm, position in meters from the floor plan origin.
     *          Whether the position lies inside the floor plan is not checked here,
     *          because the floor plan size is unknown at this point.
     *
     * @throws std::runtime_error if the file cannot be read, is not a non-empty array
     *          or any transmitter object is invalid.
     */
    [[nodiscard]] std::vector<Model::Transmitter> loadTransmitters(const std::filesystem::path &path);
}
