#include <rf_sim/io/materials_io.hpp>

#include <fstream>

#include <nlohmann/json.hpp>

namespace RfSim::Io {
    using namespace std::string_literals;

    namespace {
        // Materials config JSON key names
        constexpr auto k_name = "name";
        constexpr auto k_rgb = "rgb";
        constexpr auto k_itu = "itu";
        constexpr std::array k_ituKeys = {"a", "b", "c", "d"};

        /**
         * @brief Validates a single material JSON object and converts it to Material.
         *
         * @param json material object with "name", "rgb" and "itu" keys.
         *
         * @return Parsed material.
         *
         * @throws std::runtime_error describing the first invalid or missing field.
         */
        Model::Material parseMaterial(const nlohmann::json &json) {
            auto errorOnConditionMet = [](const bool condition, const std::string &errorMessage) {
                if (condition) throw std::runtime_error("parseMaterial: material " + errorMessage);
            };

            errorOnConditionMet(!json.is_object(), "is not an object");
            errorOnConditionMet(json.empty(), "object is empty");

            // Check name
            errorOnConditionMet(!json.contains(k_name), k_name + " is missing"s);
            errorOnConditionMet(!json.at(k_name).is_string(), k_name + " is not a string"s);

            // Check color
            errorOnConditionMet(!json.contains(k_rgb), k_rgb + " is missing"s);
            errorOnConditionMet(!json.at(k_rgb).is_array(), k_rgb + " is not an array"s);
            errorOnConditionMet(json.at(k_rgb).size() != 3, k_rgb + " size must be 3"s);
            for (const auto &value: json.at(k_rgb)) {
                errorOnConditionMet(!value.is_number_integer(), k_rgb + " value must be an integer"s);
                errorOnConditionMet(value < 0 || value > 255, k_rgb + " value must be between 0 and 255"s);
            }

            // Check itu
            errorOnConditionMet(!json.contains(k_itu), k_itu + " is missing"s);
            errorOnConditionMet(!json.at(k_itu).is_object(), k_itu + " is not an object"s);
            errorOnConditionMet(json.at(k_itu).empty(), k_itu + " object is empty"s);

            // Check itu keys
            for (const auto &key: k_ituKeys) {
                errorOnConditionMet(!json.at(k_itu).contains(key), k_itu + " key '"s + key + "' is missing");
                errorOnConditionMet(!json.at(k_itu).at(key).is_number(), k_itu + " key '"s + key + "' is not a number");
            }

            Model::Material material;
            material.name = json.at(k_name).get<std::string>();
            material.rgb = {.r = json.at(k_rgb).at(0).get<uint8_t>(),
                            .g = json.at(k_rgb).at(1).get<uint8_t>(),
                            .b = json.at(k_rgb).at(2).get<uint8_t>(),};
            material.ituModel = {.a = json.at(k_itu).at(k_ituKeys.at(0)).get<double>(),
                                 .b = json.at(k_itu).at(k_ituKeys.at(1)).get<double>(),
                                 .c = json.at(k_itu).at(k_ituKeys.at(2)).get<double>(),
                                 .d = json.at(k_itu).at(k_ituKeys.at(3)).get<double>(),};
            return material;
        }
    }

    Model::Materials loadMaterials(const std::filesystem::path &path) {
        nlohmann::json materialsConfig;
        std::ifstream file(path);

        if (!file) throw std::runtime_error("loadMaterials: cannot open " + path.string());

        try {
            materialsConfig = nlohmann::json::parse(file);
        } catch (std::exception &e) {
            throw std::runtime_error("loadMaterials: JSON parsing failed " + std::string(e.what()));
        }

        if (materialsConfig.empty())
            throw std::runtime_error("loadMaterials: materialsConfig is empty");
        if (!materialsConfig.is_array())
            throw std::runtime_error("loadMaterials: material config is not an array");

        Model::Materials materials;
        for (const auto &material: materialsConfig) {
            try {
                materials.push_back(parseMaterial(material));
            } catch (const std::exception &e) {
                throw std::runtime_error(
                    "loadMaterials: failed to parse material object '" + material.dump() + "': " +
                    std::string(e.what()));
            }
        }

        return materials;
    }


}
