rf_sim_add_library(app
        SOURCES
        src/logging_session.cpp
        src/cli_progress.cpp
        HEADERS
        include/rf_sim/app/logging_session.hpp
        include/rf_sim/app/cli_progress.hpp
        PUBLIC_DEPS
        rf_sim::solver_api
        PRIVATE_DEPS
        fmt::fmt
        indicators::indicators
)

rf_sim_add_executable(rf_sim
        SOURCES
        src/main.cpp
        DEPS
        rf_sim::app
        rf_sim::core
        fmt::fmt
)
set_target_properties(rf_sim PROPERTIES OUTPUT_NAME rf-sim)
