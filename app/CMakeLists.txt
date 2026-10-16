rf_sim_add_executable(rf_sim
        SOURCES
        main.cpp
        DEPS
        rf_sim::core
)
set_target_properties(rf_sim PROPERTIES OUTPUT_NAME rf-sim)
