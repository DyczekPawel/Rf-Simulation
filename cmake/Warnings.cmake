include_guard(GLOBAL)

# rf_sim::warnings - compiler warnings for project code.
#
# Linked PRIVATE by _rf_sim_finalize, so it does not propagate to consumers. Third-party headers
# (vcpkg imported targets) are included as system headers and do not produce warnings.
# Flags apply to C++ only, so they will not be passed to nvcc once CUDA sources are added.
option(RF_SIM_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(rf_sim_warnings INTERFACE)
add_library(rf_sim::warnings ALIAS rf_sim_warnings)

set(_rf_sim_common_warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow                # declaration shadows one from an outer scope
        -Wconversion            # implicit conversion that may lose data
        -Wsign-conversion       # signed <-> unsigned; part of -Wconversion in Clang, separate in GCC
        -Wdouble-promotion      # float implicitly promoted to double
        -Wold-style-cast        # C-style casts
        -Wcast-align            # cast increases required alignment
        -Wnon-virtual-dtor      # class with virtual functions and a non-virtual destructor
        -Woverloaded-virtual    # function hides (instead of overriding) a virtual function
        -Wimplicit-fallthrough  # switch case falls through without [[fallthrough]]
        -Wformat=2              # printf-style format issues
)

set(_rf_sim_gcc_warnings
        -Wduplicated-cond       # repeated condition in an if / else if chain
        -Wduplicated-branches   # if / else branches with identical code
        -Wlogical-op            # logical operator where a bitwise one was probably meant
        -Wsuggest-override      # override without the override keyword
)

set(_rf_sim_msvc_warnings
        /W4
        /permissive-
)

target_compile_options(rf_sim_warnings INTERFACE
        "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:${_rf_sim_common_warnings}>"
        "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:${_rf_sim_gcc_warnings}>"
        "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:${_rf_sim_msvc_warnings}>"
)

if (RF_SIM_WARNINGS_AS_ERRORS)
    target_compile_options(rf_sim_warnings INTERFACE
            "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-Werror>"
            "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/WX>"
    )
endif ()
