add_library(rf_sim_logging INTERFACE)
add_library(RfSim::logging ALIAS rf_sim_logging)
target_link_libraries(rf_sim_logging INTERFACE spdlog::spdlog)
target_compile_definitions(rf_sim_logging INTERFACE
        SPDLOG_ACTIVE_LEVEL=$<IF:$<CONFIG:Debug>,SPDLOG_LEVEL_TRACE,SPDLOG_LEVEL_INFO>)