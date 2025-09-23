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

option(VKWP_SPDLOG_LOGGING ON "Use spdlog for logging")
if (${VKWP_SPDLOG_LOGGING})
    find_package(spdlog 1 REQUIRED)
endif()

add_compile_options(
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>"
    "$<$<CXX_COMPILER_ID:MSVC>:/MP>"

    # displays level 1, level 2, and level 3 warnings, and all level 4 (informational) warnings
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"

    # Treats all compiler warnings as errors.
    "$<$<CXX_COMPILER_ID:MSVC>:/WX>"
    
    # 'enumeration': a forward declaration of an unscoped enumeration must have an underlying type
    "$<$<CXX_COMPILER_ID:MSVC>:/wd4471>"
)

add_subdirectory(vkwp)

add_doxygen_target()
export_config()
