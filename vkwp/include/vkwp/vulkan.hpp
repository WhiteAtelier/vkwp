// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_VULKAN_HPP
#define VKWP_VULKAN_HPP

// before include vulkan.
#if VKWP_USE_WIN32_SURFACE && defined(_WIN32) && _WIN32
#    define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

#endif