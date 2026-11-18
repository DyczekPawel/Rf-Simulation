include_guard(GLOBAL)

option(RF_SIM_ENABLE_ASAN "Address + undefined behavior sanitizer" OFF)
option(RF_SIM_ENABLE_TSAN "Thread sanitizer" OFF)
option(RF_SIM_ENABLE_COVERAGE "Code coverage" OFF)

if (RF_SIM_ENABLE_ASAN AND RF_SIM_ENABLE_TSAN)
    message(FATAL_ERROR "ASan and TSan cannot be enabled simultaneously")
endif ()

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND (RF_SIM_ENABLE_ASAN OR RF_SIM_ENABLE_TSAN))
    message(FATAL_ERROR "Sanitizers are not available for GCC/MinGW, use a Linux (WSL) preset")
endif ()
if (WIN32 AND RF_SIM_ENABLE_TSAN)
    message(FATAL_ERROR "TSan is not supported on Windows")
endif ()

if (RF_SIM_ENABLE_COVERAGE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Coverage with optimizations enabled gives inaccurate line results")
endif ()

add_library(rf_sim_build_utils INTERFACE)
add_library(rf_sim::build_utils ALIAS rf_sim_build_utils)

if (RF_SIM_ENABLE_ASAN)
    target_compile_options(rf_sim_build_utils INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(rf_sim_build_utils INTERFACE -fsanitize=address,undefined)
endif ()

if (RF_SIM_ENABLE_TSAN)
    target_compile_options(rf_sim_build_utils INTERFACE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(rf_sim_build_utils INTERFACE -fsanitize=thread)
endif ()

if (RF_SIM_ENABLE_COVERAGE)
    target_compile_options(rf_sim_build_utils INTERFACE --coverage)
    target_link_options(rf_sim_build_utils INTERFACE --coverage)
endif ()