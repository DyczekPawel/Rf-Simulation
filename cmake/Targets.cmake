include_guard(GLOBAL)

# Applies project-wide settings to a target: C++26 and an IDE folder for every target;
# warnings, sanitizers/coverage (rf_sim::build_utils) and clang-tidy for compiled targets only,
# since an interface library has nothing to compile.
#
#   _rf_sim_finalize(<target> <folder>)
#
#   <folder>  IDE folder (FOLDER property); an empty string leaves the target at the top level.
#
# Internal: use the rf_sim_add_* helpers instead of calling this directly.
function(_rf_sim_finalize TARGET FOLDER)
    get_target_property(type ${TARGET} TYPE)
    if (type STREQUAL "INTERFACE_LIBRARY")
        target_compile_features(${TARGET} INTERFACE cxx_std_26)
    else ()
        target_compile_features(${TARGET} PUBLIC cxx_std_26)
        target_link_libraries(${TARGET} PRIVATE rf_sim::warnings rf_sim_logging rf_sim::build_utils)
        rf_sim_enable_clang_tidy(${TARGET})
    endif ()

    if (NOT FOLDER STREQUAL "")
        set_target_properties(${TARGET} PROPERTIES FOLDER "${FOLDER}")
    endif ()
endfunction()

# Creates library target rf_sim_<name> with alias rf_sim::<name>.
#
#   rf_sim_add_library(<name>
#       HEADERS <files>...
#       [SOURCES <files>...]
#       [PRIVATE_HEADERS <files>...]
#       [PUBLIC_DEPS <targets>...]
#       [PRIVATE_DEPS <targets>...]
#       [FOLDER <folder>])
#
#   HEADERS          Public headers under include/. Consumers include them relative to include/,
#                    e.g. include/rf_sim/io/image_io.hpp -> #include <rf_sim/io/image_io.hpp>.
#   SOURCES          Without sources the library is header-only (INTERFACE);
#                    PRIVATE_HEADERS and PRIVATE_DEPS are then rejected, since there is nothing private to use them.
#   PRIVATE_HEADERS  Internal headers under src/, visible only to the library's own sources.
#   PUBLIC_DEPS      Linked PUBLIC (INTERFACE for header-only): propagated to consumers.
#   PRIVATE_DEPS     Linked PRIVATE: implementation detail, not visible to consumers.
#   FOLDER           IDE folder, defaults to RF_SIM_LIBRARIES_TARGETS_FOLDER.
#
# Paths are relative to the calling CMakeLists.txt, which must contain the include/ and src/ dirs.
function(rf_sim_add_library NAME)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "FOLDER"
            "SOURCES;HEADERS;PRIVATE_HEADERS;PUBLIC_DEPS;PRIVATE_DEPS")

    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "rf_sim_add_library(${NAME}): unexpected arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif ()
    if (ARG_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "rf_sim_add_library(${NAME}): keywords without values: ${ARG_KEYWORDS_MISSING_VALUES}")
    endif ()
    if (NOT ARG_HEADERS)
        message(FATAL_ERROR "rf_sim_add_library(${NAME}): missing HEADERS")
    endif ()

    set(TARGET rf_sim_${NAME})

    if (ARG_SOURCES)
        add_library(${TARGET} ${ARG_SOURCES})
        set(SCOPE PUBLIC)
    else ()
        foreach (bad IN ITEMS PRIVATE_DEPS PRIVATE_HEADERS)
            if (ARG_${bad})
                message(FATAL_ERROR "rf_sim_add_library(${NAME}): ${bad} not allowed for header-only library")
            endif ()
        endforeach ()
        add_library(${TARGET} INTERFACE)
        set(SCOPE INTERFACE)
    endif ()
    add_library(rf_sim::${NAME} ALIAS ${TARGET})

    target_sources(${TARGET} ${SCOPE}
            FILE_SET HEADERS
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
            FILES ${ARG_HEADERS}
    )
    if (ARG_PRIVATE_HEADERS)
        target_sources(${TARGET} PRIVATE
                FILE_SET private_headers TYPE HEADERS
                BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
                FILES ${ARG_PRIVATE_HEADERS}
        )
    endif ()

    if (ARG_PUBLIC_DEPS)
        target_link_libraries(${TARGET} ${SCOPE} ${ARG_PUBLIC_DEPS})
    endif ()
    if (ARG_PRIVATE_DEPS)
        target_link_libraries(${TARGET} PRIVATE ${ARG_PRIVATE_DEPS})
    endif ()

    if (NOT DEFINED ARG_FOLDER)
        set(ARG_FOLDER "${RF_SIM_LIBRARIES_TARGETS_FOLDER}")
    endif ()
    _rf_sim_finalize(${TARGET} "${ARG_FOLDER}")
endfunction()

# Creates executable target <name> (no prefix, no alias) with project-wide settings.
#
#   rf_sim_add_executable(<name>
#       SOURCES <files>...
#       [DEPS <targets>...]
#       [FOLDER <folder>])
#
#   DEPS    Linked PRIVATE.
#   FOLDER  IDE folder, defaults to the top level.
#
# Executables go through _rf_sim_finalize like libraries, so their own sources also get warnings,
# sanitizer instrumentation and clang-tidy.
function(rf_sim_add_executable NAME)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "FOLDER" "SOURCES;DEPS")

    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "rf_sim_add_executable(${NAME}): unexpected arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif ()
    if (NOT ARG_SOURCES)
        message(FATAL_ERROR "rf_sim_add_executable(${NAME}): missing SOURCES")
    endif ()

    add_executable(${NAME} ${ARG_SOURCES})
    if (ARG_DEPS)
        target_link_libraries(${NAME} PRIVATE ${ARG_DEPS})
    endif ()

    _rf_sim_finalize(${NAME} "${ARG_FOLDER}")
endfunction()