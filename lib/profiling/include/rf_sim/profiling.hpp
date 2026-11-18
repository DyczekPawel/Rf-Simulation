#pragma once

#ifdef RF_SIM_PROFILING

#include <cstdint>
#include <type_traits>

#include <tracy/Tracy.hpp>

namespace RfSim::Profiling::detail {
/// TracyPlot only accepts int64_t / float / double; size_t etc. would be ambiguous.
    template <typename T>
    inline void plot(const char *name, T value) {
        static_assert(std::is_arithmetic_v<T>, "RF_SIM_PROFILE_PLOT expects a number");
        if constexpr (std::is_integral_v<T>) {
            TracyPlot(name, static_cast<std::int64_t>(value));
        } else {
            TracyPlot(name, static_cast<double>(value));
        }
    }
} // namespace RfSim::Profiling::detail

/// Zone covering the enclosing scope, named after the enclosing function.
#define RF_SIM_PROFILE_FUNCTION() ZoneScoped
/// Zone covering the enclosing scope with an explicit name (string literal).
#define RF_SIM_PROFILE_SCOPE(name) ZoneScopedN(name)
/// Numeric value sampled over time, drawn as a plot under the timeline.
#define RF_SIM_PROFILE_PLOT(name, value) ::RfSim::Profiling::detail::plot(name, value)
/// Point-in-time marker on the timeline (string literal).
#define RF_SIM_PROFILE_MESSAGE(text) TracyMessageL(text)
/// Name of the calling thread as shown in the profiler (string literal).
#define RF_SIM_PROFILE_THREAD_NAME(name) ::tracy::SetThreadName(name)

#else

#define RF_SIM_PROFILE_FUNCTION() static_cast<void>(0)
#define RF_SIM_PROFILE_SCOPE(name) static_cast<void>(0)
// sizeof keeps the operand unevaluated but still "used" - no -Wunused warnings
// for variables that exist only to be plotted.
#define RF_SIM_PROFILE_PLOT(name, value) static_cast<void>(sizeof(value))
#define RF_SIM_PROFILE_MESSAGE(text) static_cast<void>(0)
#define RF_SIM_PROFILE_THREAD_NAME(name) static_cast<void>(0)

#endif
