cmake_minimum_required(VERSION 3.23)

project(vkwp
    VERSION
        0.1.0
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

option(VKWP_SPDLOG_LOGGING OFF "Use spdlog for logging")
if (${VKWP_SPDLOG_LOGGING})
    find_package(spdlog 1 REQUIRED)
endif()

add_compile_options(
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>"
    "$<$<CXX_COMPILER_ID:MSVC>:/MP>"
)

add_subdirectory(vkwp)

add_doxygen_target()
export_config()
