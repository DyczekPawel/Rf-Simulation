add_library(rf_sim_profiling INTERFACE)
target_include_directories(rf_sim_profiling INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (RF_SIM_PROFILING)
    target_link_libraries(rf_sim_profiling INTERFACE Tracy::TracyClient)
    target_compile_definitions(rf_sim_profiling INTERFACE RF_SIM_PROFILING)

    add_library(rf_sim_profiling_memory_hooks OBJECT
            src/memory_hooks.cpp
    )
    target_link_libraries(rf_sim_profiling_memory_hooks PRIVATE Tracy::TracyClient)
    target_compile_definitions(rf_sim_profiling_memory_hooks PRIVATE
            RF_SIM_PROFILING_ALLOC_CALLSTACK_DEPTH=${RF_SIM_PROFILING_ALLOC_CALLSTACK_DEPTH}
    )

    if (TARGET opencv_core)
        add_library(rf_sim_profiling_opencv STATIC
                src/opencv_memory.cpp
        )
        target_link_libraries(rf_sim_profiling_opencv
                PUBLIC rf_sim_profiling
                PRIVATE opencv_core
        )
    endif ()
endif ()
