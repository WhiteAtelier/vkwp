# Copyright (c) 2026 Shirao Shotaro, WhiteAtelier
# SPDX-License-Identifier: MIT
#
# See the LICENSE file in the project root for the full license text.

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "WhiteAtelier")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(VKWP_CPACK_PACKAGE_DIRECTORY "${PROJECT_SOURCE_DIR}/cpack" CACHE PATH "Package output directory.")
set(CPACK_PACKAGE_DIRECTORY "${VKWP_CPACK_PACKAGE_DIRECTORY}")
set(CPACK_GENERATOR "TGZ")

# MSVC ではホストではなく, ビルド対象の architecture を使用する.
set(vkwp_package_arch "${CMAKE_SYSTEM_PROCESSOR}")
if (MSVC AND MSVC_CXX_ARCHITECTURE_ID)
    set(vkwp_package_arch "${MSVC_CXX_ARCHITECTURE_ID}")
endif()

string(TOLOWER "${vkwp_package_arch}" vkwp_package_arch)
if (vkwp_package_arch MATCHES "^(amd64|x64|x86_64)$")
    set(vkwp_package_arch "x86_64")
elseif (vkwp_package_arch MATCHES "^(arm64|aarch64)$")
    set(vkwp_package_arch "aarch64")
elseif (vkwp_package_arch MATCHES "^(x86|i[3-6]86)$")
    set(vkwp_package_arch "x86")
endif()

# OS はバージョンやディストリビューションを含めず, 小文字で表記する.
string(TOLOWER "${CMAKE_SYSTEM_NAME}" vkwp_package_platform)

set(CPACK_PACKAGE_FILE_NAME
    "${PROJECT_NAME}-${PROJECT_VERSION}-${vkwp_package_platform}-${vkwp_package_arch}"
)

install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION "share/licenses/${PROJECT_NAME}")

include(CPack)
