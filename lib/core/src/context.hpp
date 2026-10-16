#pragma once
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// Private header of the core module.
namespace RfSim::Core::Detail {
    /**
     * @brief Calls fn(args...) and prefixes any thrown error message with context.
     */
    template <typename F, typename... Args>
    auto withContext(const std::string &context, F &&fn, Args &&... args) -> std::invoke_result_t<F, Args...> {
        try {
            return std::invoke(std::forward<F>(fn), std::forward<Args>(args)...);
        } catch (const std::exception &e) {
            throw std::runtime_error(context + ": " + e.what());
        }
    }
}
