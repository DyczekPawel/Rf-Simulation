cmake_minimum_required(VERSION 3.31)

option(RF_SIM_BUILD_TESTS "Build tests" ON)
if (RF_SIM_BUILD_TESTS)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif ()

project(rf_sim VERSION 0.1.0 DESCRIPTION "RF Simulation" LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Optimization options
option(RF_SIM_ENABLE_LTO "Enable link-time optimization for Release builds" ON)
set(RF_SIM_ARCH "" CACHE STRING "Value passed to -march (empty = compiler default), e.g. native, x86-64-v2")

include(CheckIPOSupported)
check_ipo_supported(RESULT RF_SIM_IPO_SUPPORTED OUTPUT RF_SIM_IPO_ERROR LANGUAGES CXX)
if (RF_SIM_ENABLE_LTO AND NOT RF_SIM_IPO_SUPPORTED)
    message(WARNING "LTO requested but not supported: ${RF_SIM_IPO_ERROR}")
endif ()

# Define folders for target organization
set(RF_SIM_LIBRARIES_TARGETS_FOLDER "rf_sim_libs" CACHE STRING "Folder where libraries targets are placed")
set(RF_SIM_TESTS_TARGETS_FOLDER "rf_sim_tests" CACHE STRING "Folder where tests targets are placed")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)
include(BuildUtils)
include(ClangTidy)
include(Warnings)
include(Logging)
include(Profiling)
include(Targets)

add_subdirectory(lib)
add_subdirectory(app)

if (RF_SIM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()