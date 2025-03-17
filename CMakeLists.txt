##############################################
# Recent version of CMake that has server mode enabled (at least 3.8).
# comment
cmake_minimum_required (VERSION 3.8)

##############################################
# CMake policy
# Use the NEW behavior for this policy and set the VERSION in the project() command
cmake_policy(SET CMP0048 NEW)

#
# Handle transitive dependency conflict that Git submodules can lead to 
# This happens when the main project has direct SDK dependency and also 
# transitive dependencies because of other toolbox dependencies
#
if(TARGET mtscpp)
    return()
endif()

project(magno_sdk)

# Append to default CMake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/toolchain)

if(CTEST_MEMORYCHECK_TYPE STREQUAL "Valgrind")
    # See https://cmake.org/cmake/help/latest/manual/ctest.1.html#ctest-memcheck-step
    # Also note that we still cannot have 100% Valgrind analysis due to a similar issue like this: https://bugs.kde.org/show_bug.cgi?id=422601
    set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_CURRENT_LIST_DIR}/.valgrind-suppressions")
endif()

include(mts-options)


#################################################
# Unit Testing
#################################################

# The SDK should be structured in a way that supports not being the top level project
# so that is the reason here we introduce SDK specific options
option(MTS_SDK_BUILD_TESTING "Build the testing tree" OFF)
option(MTS_SDK_BUILD_BENCHMARK "Build benchmarks" OFF)

# ################################################
# CI integration test support
# ################################################
if(CIP_BUILD_SYSTEM_VERSION)
    # Also enable CTest when building in CI
    include(CTest)
    set(MTS_SDK_BUILD_TESTING ON)

    # Activate AddressSanitizer in CI if possible
    # that is Linux Debug builds
    if(CMAKE_COMPILER_IS_GNUCXX AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "9.0.0")
        # Set `-fsanitize=address` for both the compiler and linker
        add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
        link_libraries($<$<CONFIG:Debug>:-fsanitize=address>)
        set(CTEST_MEMORYCHECK_TYPE "AddressSanitizer")
    endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    # Add an option to use the old C++ ABI
    option(MTS_USE_CXX98_ABI "If enabled, sets the _GLIBCXX_USE_CXX11_ABI=0 compiler flag" OFF)
    if(MTS_USE_CXX98_ABI)
        add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
    endif()
endif()

#################################################
# Static Code Analysis
#################################################
option(MTS_SDK_ENABLE_CLANG_TIDY "Run clang-tidy with the compiler" OFF) # Add clang-tidy if available

# if(CIP_BUILD_SYSTEM_VERSION)
#     set(MTS_SDK_ENABLE_CLANG_TIDY $<IF:$<CONFIG:Debug>,OFF,ON>)
# endif()

if (MTS_SDK_ENABLE_CLANG_TIDY)
    find_package(ClangTools)
endif()

# Set the MSVC flag used for system include directories
if(MSVC AND NOT CMAKE_INCLUDE_SYSTEM_FLAG_CXX)
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX -external:I)
endif()

#################################################
# Code Coverage
#################################################
option(BUILD_COVERAGE "Build test coverage" OFF)

if(BUILD_COVERAGE)
    include(CodeCoverage)
    set(CODE_COVERAGE_VERBOSE ON)
    append_coverage_compiler_flags()

    # Create lcov based HTML coverage report
    setup_target_for_coverage_lcov(
        NAME coverage-lcov-html # Name of the CMake target
        EXECUTABLE ctest -j ${PROCESSOR_COUNT} # Execute with ctest
        EXCLUDE "*/src/mts/runtime/sqlite3/sqlite3.c" "*/src/mts/runtime/sqlite3/sqlite3.h" "*/test/*" "extern/*" "${CMAKE_BINARY_DIR}/*" # Files and directories to exclude
        LCOV_ARGS "--no-external" # Additionally exclude everything external
    )
endif()

add_subdirectory(src/mtsc)
add_subdirectory(src/mtscpp)

# ################################################
# CI integration test support
# ################################################
if(CIP_BUILD_SYSTEM_VERSION)
    # Add a target that will be run after ALL (custom targets are always considered out of date)
    add_custom_target(
        run_integration_tests ALL 
        COMMAND ctest -C $<CONFIGURATION> --output-on-failure
    )
    add_dependencies(run_integration_tests
        mtscpp::mtscpp
        mtsc_test
        mtscpp_test
        mts_sdl_parser_tests
        mts_xtypes_parser_tests
    )
endif()
