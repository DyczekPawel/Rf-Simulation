include_guard(GLOBAL)

# Runs clang-tidy as part of compilation (CXX_CLANG_TIDY), so `cmake --build` reports its findings
# like compiler warnings, or fails on them with RF_SIM_CLANG_TIDY_WERROR. Meant for CI and
# command-line builds; CLion runs clang-tidy in the editor on its own and does not need this.
#
# Checks come from the nearest .clang-tidy file (root for libraries, tests/ for tests).
option(RF_SIM_ENABLE_CLANG_TIDY "Run clang-tidy while compiling project targets" OFF)
option(RF_SIM_CLANG_TIDY_WERROR "Treat clang-tidy warnings as errors" OFF)

if (RF_SIM_ENABLE_CLANG_TIDY)
    find_program(RF_SIM_CLANG_TIDY_EXE NAMES clang-tidy-23 clang-tidy DOC "clang-tidy path" REQUIRED)

    set(RF_SIM_CLANG_TIDY_COMMAND "${RF_SIM_CLANG_TIDY_EXE}")
    if (RF_SIM_CLANG_TIDY_WERROR)
        list(APPEND RF_SIM_CLANG_TIDY_COMMAND "--warnings-as-errors=*")
    endif ()

    message(STATUS "clang-tidy: ${RF_SIM_CLANG_TIDY_EXE} (werror=${RF_SIM_CLANG_TIDY_WERROR})")
endif ()

# Enables clang-tidy for <target> when RF_SIM_ENABLE_CLANG_TIDY is ON, no-op otherwise.
# Called by _rf_sim_finalize for every compiled project target.
#
#   rf_sim_enable_clang_tidy(<target>)
function(rf_sim_enable_clang_tidy TARGET)
    if (RF_SIM_ENABLE_CLANG_TIDY)
        set_target_properties(${TARGET} PROPERTIES CXX_CLANG_TIDY "${RF_SIM_CLANG_TIDY_COMMAND}")
    endif ()
endfunction()