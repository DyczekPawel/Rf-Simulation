#include <rf_sim/io/simulation_config_io.hpp>

#include "json_utils.hpp"

namespace RfSim::Io {
    namespace {
        // Simulation config JSON key names
        constexpr std::string_view k_input = "input";
        constexpr std::string_view k_output = "output";
        constexpr std::string_view k_pixelsPerMeter = "pixels_per_meter";
        constexpr std::string_view k_materials = "materials";
        constexpr std::string_view k_transmitters = "transmitters";
        constexpr std::string_view k_solver = "solver";
        constexpr std::string_view k_type = "type";
        constexpr std::string_view k_params = "params";

        // Raytracing params key names
        constexpr std::string_view k_maxBounces = "max_bounces";
        constexpr std::string_view k_minPowerDbm = "min_power_dbm";
        constexpr std::string_view k_polarization = "polarization";

        std::filesystem::path requirePath(const nlohmann::json &json, const std::string_view key) {
            const auto value = Detail::require<std::string>(json, key);
            if (value.empty()) throw std::runtime_error("'" + std::string(key) + "' must not be empty");
            return Detail::pathFromUtf8(value);
        }

        /**
         * @brief Strict enum parsing: unknown values are an error, never a fallback.
         */
        Solver::Polarization parsePolarization(const std::string &value) {
            if (value == "TE") return Solver::Polarization::TE;
            if (value == "TM") return Solver::Polarization::TM;
            throw std::runtime_error("'" + std::string(k_polarization) + "' must be \"TE\" or \"TM\", got \"" +
                                     value + "\"");
        }

        /**
         * @brief Reads the solver type name from the "solver" object.
         *
         * @details "params" is only allowed here; it is validated by the solver-specific loader.
         */
        std::string parseSolverType(const nlohmann::json &solver) {
            Detail::rejectUnknownKeys(solver, {k_type, k_params});

            auto type = Detail::require<std::string>(solver, k_type);
            if (type.empty()) throw std::runtime_error("'" + std::string(k_type) + "' must not be empty");
            return type;
        }

        /**
         * @brief Parses "solver.params" for the raytracing solver; missing keys keep the defaults.
         */
        Solver::RaytracingConfig parseRaytracingParams(const nlohmann::json &params) {
            if (!params.is_object()) throw std::runtime_error("must be an object");
            Detail::rejectUnknownKeys(params, {k_maxBounces, k_minPowerDbm, k_polarization});

            Solver::RaytracingConfig config;

            // decltype keeps the parser in sync with the field types in RaytracingConfig
            config.maxBounces = Detail::getOr<decltype(config.maxBounces)>(params, k_maxBounces, config.maxBounces);
            config.minPowerDbm = Detail::getOr<decltype(config.minPowerDbm)>(params,
                                                                             k_minPowerDbm,
                                                                             config.minPowerDbm);
            if (params.contains(k_polarization))
                config.polarization = parsePolarization(Detail::require<std::string>(params, k_polarization));

            return config;
        }
    }

    SimulationFile loadSimulationFile(const std::filesystem::path &path) {
        const auto load = [&] {
            const auto json = Detail::readJsonFile(path);
            if (!json.is_object()) throw std::runtime_error("root must be an object");
            Detail::rejectUnknownKeys(json,
                                      {k_input, k_output, k_pixelsPerMeter, k_materials, k_transmitters, k_solver});

            SimulationFile file;
            file.input.floorPlan = requirePath(json, k_input);
            file.input.pixelsPerMeter = Detail::require<double>(json, k_pixelsPerMeter);
            if (!(file.input.pixelsPerMeter > 0.0))
                throw std::runtime_error("'" + std::string(k_pixelsPerMeter) + "' must be positive");

            file.output.directory = requirePath(json, k_output);
            if (json.contains(k_materials)) file.materials = requirePath(json, k_materials);
            if (json.contains(k_transmitters)) file.transmitters = requirePath(json, k_transmitters);

            const auto &solver = Detail::requireObject(json, k_solver);
            file.solverType = Detail::withContext("'solver'", parseSolverType, solver);

            return file;
        };

        return Detail::withContext("loadSimulationFile: " + path.string(), load);
    }

    Solver::RaytracingConfig loadRaytracingConfig(const std::filesystem::path &path) {
        const auto load = [&] {
            const auto json = Detail::readJsonFile(path);
            const auto &solver = Detail::requireObject(json, k_solver);

            const auto paramsIt = solver.find(k_params);
            if (paramsIt == solver.end()) return Solver::RaytracingConfig{}; // all defaults

            return Detail::withContext("'solver.params'", parseRaytracingParams, *paramsIt);
        };

        return Detail::withContext("loadRaytracingConfig: " + path.string(), load);
    }
}
