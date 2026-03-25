cmake_minimum_required(VERSION 3.23)

project(vkwp
    VERSION
        2.0.2
    DESCRIPTION
        "Vulkan Wrapper"
    LANGUAGES
        CXX
)

include(cmake/doxygen/doxygen.cmake)
include(cmake/utils.cmake)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Debug postfix")

# Dependency
find_package(Vulkan 1.4 REQUIRED)
find_package(spdlog CONFIG REQUIRED)

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
