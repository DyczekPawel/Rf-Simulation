include_guard(GLOBAL)

option(RF_SIM_PROFILING "Instrument the build with Tracy (CPU zones + heap tracking)" OFF)

set(RF_SIM_TRACY_VERSION "v0.15.0" CACHE STRING "Tracy git tag.")

set(RF_SIM_PROFILING_ALLOC_CALLSTACK_DEPTH "0" CACHE STRING
        "Call stack depth captured for every heap allocation (0 = off). \
On Windows symbols are resolved through dbghelp, so this needs PDB debug info (clang -gcodeview); \
GCC/DWARF builds will show unresolved frames.")

if (WIN32)
    set(_rf_sim_ignore_faults_default ON)
else ()
    set(_rf_sim_ignore_faults_default OFF)
endif ()

option(RF_SIM_PROFILING_IGNORE_MEMORY_FAULTS
        "Tell Tracy to ignore unmatched alloc/free events instead of aborting the capture"
        ${_rf_sim_ignore_faults_default})
unset(_rf_sim_ignore_faults_default)

function(_rf_sim_fetch_tracy)
    include(FetchContent)

    set(TRACY_ENABLE ON)
    set(TRACY_STATIC ON)
    set(TRACY_ONLY_LOCALHOST ON)
    #    set(TRACY_NO_EXIT ON)
    set(TRACY_IGNORE_MEMORY_FAULTS ${RF_SIM_PROFILING_IGNORE_MEMORY_FAULTS})

    FetchContent_Declare(tracy
            GIT_REPOSITORY https://github.com/wolfpld/tracy.git
            GIT_TAG ${RF_SIM_TRACY_VERSION}
            GIT_SHALLOW TRUE
            SYSTEM
    )
    FetchContent_MakeAvailable(tracy)
endfunction()

if (RF_SIM_PROFILING)
    _rf_sim_fetch_tracy()
    message(STATUS "RF-Sim: Tracy profiling ENABLED (${RF_SIM_TRACY_VERSION})")
endif ()

function(rf_sim_enable_memory_profiling target)
    if (RF_SIM_PROFILING)
        target_link_libraries(${target} PRIVATE rf_sim_profiling_memory_hooks)
        if (TARGET rf_sim_profiling_opencv)
            target_link_libraries(${target} PRIVATE rf_sim_profiling_opencv)
        endif ()
    endif ()
endfunction()
