#pragma once
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <nlohmann/json.hpp>

// Private header of the io module: nlohmann::json must not leak into public headers.
namespace RfSim::Io::Detail {
    /**
     * @brief Reads and parses a JSON file.
     *
     * @details Error messages are short; callers add the file and function via withContext.
     *
     * @throws std::runtime_error if the file cannot be opened or is not valid JSON.
     */
    [[nodiscard]] nlohmann::json readJsonFile(const std::filesystem::path &path);

    /**
     * @brief Calls fn(args...) and prefixes any thrown error message with context.
     *
     * @details Lets inner helpers throw short messages ("'x' is missing") while the outer
     *          loader adds where it happened ("loadTransmitters: tx.json: transmitter [2]: ...").
     *          Accepts any callable, so both a named lambda and a plain function with its
     *          arguments can be passed: withContext("'solver'", parseSolverType, solverJson).
     */
    template <typename F, typename... Args>
    auto withContext(const std::string &context, F &&fn, Args &&... args) -> std::invoke_result_t<F, Args...> {
        try {
            return std::invoke(std::forward<F>(fn), std::forward<Args>(args)...);
        } catch (const std::exception &e) {
            throw std::runtime_error(context + ": " + e.what());
        }
    }

    /**
     * @brief Converts a UTF-8 string (JSON strings are always UTF-8) to a filesystem path.
     *
     * @details std::filesystem::path(std::string) interprets the string in the native narrow
     *          encoding, which on Windows is the ANSI code page, not UTF-8. Non-ASCII paths
     *          (e.g. "plan_piętro.png") would be silently mangled without this conversion.
     */
    [[nodiscard]] inline std::filesystem::path pathFromUtf8(const std::string &utf8) {
        return {std::u8string(utf8.begin(), utf8.end())};
    }

    template <typename T>
    [[nodiscard]] constexpr std::string_view typeDescription() {
        if constexpr (std::same_as<T, bool>) return "a boolean";
        else if constexpr (std::unsigned_integral<T>) return "a non-negative integer within range";
        else if constexpr (std::integral<T>) return "an integer within range";
        else if constexpr (std::floating_point<T>) return "a number";
        else return "a string";
    }

    /**
     * @brief Checks whether json holds a value convertible to T without loss.
     *
     * @details Integers are range-checked, so e.g. -1 is rejected for unsigned and 3.5 for int
     *          instead of being silently wrapped or truncated by json::get<T>().
     */
    template <typename T>
    [[nodiscard]] bool holdsType(const nlohmann::json &json) {
        if constexpr (std::same_as<T, bool>) {
            return json.is_boolean();
        } else if constexpr (std::integral<T>) {
            if (json.is_number_unsigned()) return std::in_range<T>(json.get<std::uint64_t>());
            if (json.is_number_integer()) return std::in_range<T>(json.get<std::int64_t>());
            return false;
        } else if constexpr (std::floating_point<T>) {
            return json.is_number();
        } else if constexpr (std::same_as<T, std::string>) {
            return json.is_string();
        } else {
            static_assert(sizeof(T) == 0, "holdsType: unsupported type");
        }
        std::unreachable();
    }

    /**
     * @brief Returns json[key] converted to T.
     *
     * @throws std::runtime_error if the key is missing or the value has the wrong type / range.
     */
    template <typename T>
    [[nodiscard]] T require(const nlohmann::json &json, const std::string_view key) {
        const auto it = json.find(key);
        if (it == json.end())
            throw std::runtime_error("'" + std::string(key) + "' is missing");
        if (!holdsType<T>(*it))
            throw std::runtime_error("'" + std::string(key) + "' must be " + std::string(typeDescription<T>()));
        return it->get<T>();
    }

    /**
     * @brief Like require, but returns defaultValue when the key is absent.
     *
     * @details A present key with a wrong type is still an error, never a silent fallback.
     */
    template <typename T>
    [[nodiscard]] T getOr(const nlohmann::json &json, const std::string_view key, T defaultValue) {
        if (!json.contains(key)) return defaultValue;
        return require<T>(json, key);
    }

    /**
     * @brief Returns json[key], which must exist and be a JSON object.
     */
    [[nodiscard]] inline const nlohmann::json &requireObject(const nlohmann::json &json, const std::string_view key) {
        const auto it = json.find(key);
        if (it == json.end())
            throw std::runtime_error("'" + std::string(key) + "' is missing");
        if (!it->is_object())
            throw std::runtime_error("'" + std::string(key) + "' must be an object");
        return *it;
    }

    /**
     * @brief Rejects keys outside the allowed set.
     *
     * @details Needed wherever keys are optional: without it a typo ("max_bouncess") is
     *          silently ignored and the default value is used instead.
     */
    inline void rejectUnknownKeys(const nlohmann::json &json, const std::initializer_list<std::string_view> allowed) {
        for (const auto &item: json.items()) {
            if (std::ranges::find(allowed, std::string_view{item.key()}) == allowed.end())
                throw std::runtime_error("unknown key '" + item.key() + "'");
        }
    }
}
