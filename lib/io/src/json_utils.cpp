#include "json_utils.hpp"

#include <rf_sim/profiling.hpp>

#include <fstream>

namespace RfSim::Io::Detail {
    nlohmann::json readJsonFile(const std::filesystem::path &path) {
        RF_SIM_PROFILE_FUNCTION();

        std::ifstream file(path);
        if (!file) throw std::runtime_error("cannot open file");

        try {
            return nlohmann::json::parse(file);
        } catch (const nlohmann::json::parse_error &e) {
            throw std::runtime_error("invalid JSON: " + std::string(e.what()));
        }
    }
}
