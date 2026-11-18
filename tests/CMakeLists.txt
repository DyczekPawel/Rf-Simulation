find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

set(RF_SIM_TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

# Applies test settings to <target>: GTest (with gtest_main), the given libraries, include path
# for shared helpers (#include <common/...>), RF_SIM_TEST_DATA_DIR and project-wide settings.
#
#   _rf_sim_setup_test_target(<target> [LIBS <targets>...])
function(_rf_sim_setup_test_target TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "LIBS")

    target_link_libraries(${TARGET} PRIVATE ${ARG_LIBS} GTest::gtest GTest::gmock GTest::gtest_main)
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR})
    target_compile_definitions(${TARGET} PRIVATE RF_SIM_TEST_DATA_DIR="${RF_SIM_TEST_DATA_DIR}")
    set_target_properties(${TARGET} PROPERTIES FOLDER "${TESTS_TARGETS_FOLDER}")
    _rf_sim_finalize(${TARGET} ${RF_SIM_TESTS_TARGETS_FOLDER})
endfunction()

# Creates test executable rf_sim_<group>_tests and registers its tests in CTest
# as "<group>.<Suite>.<Test>" with label <group> (ctest -L <group>, ctest -R "^<group>\.").
# Sources are also collected into rf_sim_all_tests.
#
#   rf_sim_add_test_group(<group>
#       TESTS <names>...
#       [LIBS <targets>...])
#
#   TESTS  Test file names without the .cpp extension, relative to the calling directory.
#   LIBS   Libraries under test; linking only these checks the group does not depend on others.
function(rf_sim_add_test_group GROUP)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "TESTS;LIBS")

    set(SOURCES "")
    foreach (NAME IN LISTS ARG_TESTS)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cpp")
    endforeach ()

    set(TARGET rf_sim_${GROUP}_tests)
    add_executable(${TARGET} ${SOURCES})
    _rf_sim_setup_test_target(${TARGET} LIBS ${ARG_LIBS})

    gtest_discover_tests(${TARGET}
            TEST_PREFIX "${GROUP}."
            PROPERTIES LABELS "${GROUP}"
    )

    set_property(GLOBAL APPEND PROPERTY RF_SIM_TEST_SOURCES ${SOURCES})
    set_property(GLOBAL APPEND PROPERTY RF_SIM_TEST_LIBS ${ARG_LIBS})
endfunction()

add_subdirectory(model)
add_subdirectory(io)

get_property(ALL_SOURCES GLOBAL PROPERTY RF_SIM_TEST_SOURCES)
get_property(ALL_LIBS GLOBAL PROPERTY RF_SIM_TEST_LIBS)
list(REMOVE_DUPLICATES ALL_LIBS)

if (ALL_SOURCES)
    add_executable(rf_sim_all_tests ${ALL_SOURCES})
    _rf_sim_setup_test_target(rf_sim_all_tests LIBS ${ALL_LIBS})
endif ()