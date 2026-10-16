rf_sim_add_library(visualization
        SOURCES
        src/render.cpp
        HEADERS
        include/rf_sim/visualization/render.hpp
        PUBLIC_DEPS
        rf_sim::model
)