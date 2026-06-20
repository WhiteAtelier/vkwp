cmake_minimum_required(VERSION 3.23)

project(vkwp
    VERSION
        2.2.0
    DESCRIPTION
        "Vulkan Wrapper"
    LANGUAGES
        CXX
)

if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    set(VKWP_IS_MASTER_PROJECT ON)
else()
    set(VKWP_IS_MASTER_PROJECT OFF)
endif()

option(VKWP_BUILD_TESTS "Build test project."  OFF)
option(VKWP_BUILD_DOCS  "Build documentation." OFF)
option(VKWP_INSTALL     "Install the project." ${VKWP_IS_MASTER_PROJECT})

include(cmake/doxygen/doxygen.cmake)
include(cmake/utils.cmake)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Debug postfix")

# Dependency
if (VKWP_BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
endif()

if (VKWP_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
endif()

find_package(Vulkan 1.4 REQUIRED)
find_package(spdlog     REQUIRED)

add_compile_options(
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>"
    "$<$<CXX_COMPILER_ID:MSVC>:/MP>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/WX>"

    "$<$<CXX_COMPILER_ID:GNU>:-Wall>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wextra>"
    "$<$<CXX_COMPILER_ID:GNU>:-Werror>"
)

add_subdirectory(vkwp)

add_doxygen_target()
export_config()
