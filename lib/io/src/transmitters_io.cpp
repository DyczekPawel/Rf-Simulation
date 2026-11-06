#include <rf_sim/io/transmitters_io.hpp>
#include <rf_sim/profiling.hpp>

#include "json_utils.hpp"

namespace RfSim::Io {
    namespace {
        // Transmitters config JSON key names
        constexpr std::string_view k_name = "name";
        constexpr std::string_view k_properties = "properties";
        constexpr std::string_view k_frequency = "frequency";
        constexpr std::string_view k_powerDbm = "dBm";
        constexpr std::string_view k_position = "position";
        constexpr std::string_view k_x = "x";
        constexpr std::string_view k_y = "y";

        struct Properties {
            double frequencyHz;
            double powerDbm;
        };

        struct Position {
            double x;
            double y;
        };

        Properties parseProperties(const nlohmann::json &json) {
            Detail::rejectUnknownKeys(json, {k_frequency, k_powerDbm});

            const auto frequency = Detail::require<double>(json, k_frequency);
            if (!(frequency > 0.0))
                throw std::runtime_error("'" + std::string(k_frequency) + "' must be positive");

            return {.frequencyHz = frequency, .powerDbm = Detail::require<double>(json, k_powerDbm)};
        }

        Position parsePosition(const nlohmann::json &json) {
            Detail::rejectUnknownKeys(json, {k_x, k_y});
            return {.x = Detail::require<double>(json, k_x), .y = Detail::require<double>(json, k_y)};
        }

        Model::Transmitter parseTransmitter(const nlohmann::json &json) {
            if (!json.is_object()) throw std::runtime_error("is not an object");
            Detail::rejectUnknownKeys(json, {k_name, k_properties, k_position});

            auto name = Detail::require<std::string>(json, k_name);

            const auto [frequencyHz, powerDbm] =
                Detail::withContext("'properties'", parseProperties, Detail::requireObject(json, k_properties));

            const auto [x, y] =
                Detail::withContext("'position'", parsePosition, Detail::requireObject(json, k_position));

            return Model::Transmitter{std::move(name),
                                      {.x = x, .y = y},
                                      frequencyHz,
                                      powerDbm};
        }
    }

    std::vector<Model::Transmitter> loadTransmitters(const std::filesystem::path &path) {
        RF_SIM_PROFILE_FUNCTION();

        const auto load = [&] {
            const auto json = Detail::readJsonFile(path);
            if (!json.is_array()) throw std::runtime_error("root must be an array");
            if (json.empty()) throw std::runtime_error("no transmitters defined");

            std::vector<Model::Transmitter> transmitters;
            transmitters.reserve(json.size());
            for (size_t i = 0; i < json.size(); ++i) {
                const auto context = "transmitter [" + std::to_string(i) + "]";
                transmitters.push_back(Detail::withContext(context, parseTransmitter, json.at(i)));
            }
            return transmitters;
        };

        return Detail::withContext("loadTransmitters: " + path.string(), load);
    }
}
