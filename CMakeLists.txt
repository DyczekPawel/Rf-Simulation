cmake_minimum_required(VERSION 3.31)

option(RF_SIM_BUILD_TESTS "Build tests" ON)
if (RF_SIM_BUILD_TESTS)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif ()

project(rf_sim VERSION 0.1.0 DESCRIPTION "RF Simulation" LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Define folders for target organization
set(RF_SIM_LIBRARIES_TARGETS_FOLDER "rf_sim_libs" CACHE STRING "Folder where libraries targets are placed")
set(RF_SIM_TESTS_TARGETS_FOLDER "rf_sim_tests" CACHE STRING "Folder where tests targets are placed")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)
include(BuildUtils)
include(ClangTidy)
include(Warnings)
include(Logging)
include(Targets)

add_subdirectory(lib)
add_subdirectory(app)

if (RF_SIM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()