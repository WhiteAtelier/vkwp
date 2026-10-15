# Copyright (c) 2026 Shirao Shotaro, WhiteAtelier
# SPDX-License-Identifier: MIT
#
# See the LICENSE file in the project root for the full license text.

function(regist_doxygen_files ARG_FILES)
    if (NOT ${VKWP_BUILD_DOCS})
        return()
    endif()

    get_property(VKWP_DOXYGEN_HEADER_FILES GLOBAL PROPERTY doxygen_source_list)
    set_property(GLOBAL PROPERTY doxygen_source_list ${VKWP_DOXYGEN_HEADER_FILES} ${ARG_FILES})
endfunction()

function(add_doxygen_target)
    if (NOT ${VKWP_BUILD_DOCS})
        return()
    endif()

    find_package(Doxygen REQUIRED)

    get_property(VKWP_DOXYGEN_HEADER_FILES GLOBAL PROPERTY doxygen_source_list)
    string(REPLACE ";" " " VKWP_DOXYGEN_HEADER_FILES_S "${VKWP_DOXYGEN_HEADER_FILES}")

    set(DOXYFILE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen)
    set(DOCUMENT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    message(TRACE "--DOXYFILE_OUTPUT_DIRECTORY: " ${DOXYFILE_OUTPUT_DIRECTORY})
    message(TRACE "--DOCUMENT_OUTPUT_DIRECTORY: " ${DOCUMENT_OUTPUT_DIRECTORY})
    message(TRACE "--Doxyfile.in : " ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Doxyfile.in)
    message(TRACE "--Doxyfile    : " ${DOXYFILE_OUTPUT_DIRECTORY}/Doxyfile)
    message(TRACE "--Document    : " ${DOCUMENT_OUTPUT_DIRECTORY}/index.html)

    file(MAKE_DIRECTORY ${DOXYFILE_OUTPUT_DIRECTORY})
    file(MAKE_DIRECTORY ${DOCUMENT_OUTPUT_DIRECTORY})

    add_custom_target(doxygen
        DEPENDS ${DOCUMENT_OUTPUT_DIRECTORY}/index.html
    )

    # Generate Doxyfile from Doxyfile.in
    add_custom_command(
        COMMAND
            ${CMAKE_COMMAND}
                -D "DOXYGEN_TEMPLATE=${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Doxyfile.in"
                -D "DOXYGEN_DOXYFILE=${DOXYFILE_OUTPUT_DIRECTORY}/Doxyfile"
                -D "DOXYGEN_PROJECT_NAME=${PROJECT_NAME}"
                -D "DOXYGEN_PROJECT_DESC=${CMAKE_PROJECT_DESCRIPTION}"
                -D "DOXYGEN_PROJECT_VERSION=${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}"
                -D "DOXYGEN_INPUT_SOURCES=${VKWP_DOXYGEN_HEADER_FILES_S}"
                -D "DOXYGEN_PROJECT_INCLUDE_DIR=${CMAKE_SOURCE_DIR}/include"
                -D "DOXYGEN_SOURCE_DIR=${CMAKE_CURRENT_FUNCTION_LIST_DIR}"
                -D "DOXYGEN_README_MD=${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../readme.md"
                -D "DOCUMENT_OUTPUT_PATH=${DOCUMENT_OUTPUT_DIRECTORY}"
                -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/doxygen-script.cmake"
        DEPENDS
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Doxyfile.in"
            "${VKWP_DOXYGEN_HEADER_FILES}"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../readme.md"
        WORKING_DIRECTORY
            "${DOXYFILE_OUTPUT_DIRECTORY}"
        OUTPUT
            "${DOXYFILE_OUTPUT_DIRECTORY}/Doxyfile"
        COMMENT
            "Generating Doxyfile"
    )

    # Generate doc index.html
    add_custom_command(
        COMMAND
            ${DOXYGEN_EXECUTABLE}
            ${DOXYFILE_OUTPUT_DIRECTORY}/Doxyfile
        DEPENDS
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/icon.png
            ${DOXYFILE_OUTPUT_DIRECTORY}/Doxyfile
        WORKING_DIRECTORY
            ${DOXYFILE_OUTPUT_DIRECTORY}
        OUTPUT
            ${DOCUMENT_OUTPUT_DIRECTORY}/index.html
        COMMENT
            "Creating HTML documentation"
    )

    install(
        DIRECTORY
            ${DOCUMENT_OUTPUT_DIRECTORY}
        DESTINATION
            .
    )

endfunction()