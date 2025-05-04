#ifndef VKWP_VULKAN_HPP
#define VKWP_VULKAN_HPP

// before include vulkan.
#if VKWP_SUPPORT_WIN32_SURFACE && defined(_WIN32) && _WIN32
#    define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

#endif