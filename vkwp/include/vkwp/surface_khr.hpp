// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_SURFACE_KHR_HPP
#define VKWP_SURFACE_KHR_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>

#if VKWP_SUPPORT_GLFW_SURFACE && defined(VKWP_USE_GLFW_SURFACE)
struct GLFWwindow;
#endif

#if VKWP_SUPPORT_WIN32_SURFACE && defined(_WIN32) && defined(VKWP_USE_WIN32_SURFACE)
#    include <Windows.h>
#endif

namespace vkwp {

///
/// @brief VkSurfaceKHR wrapper.
///
class SurfaceKHR final : public Resource<VkSurfaceKHR, PFN_vkDestroySurfaceKHR, VkInstance>
{
public:
    using Resource<VkSurfaceKHR, PFN_vkDestroySurfaceKHR, VkInstance>::Resource;

    [[nodiscard]]
    static SurfaceKHR
    importExternal(std::string label, const Instance & instance, const VkSurfaceKHR external);

#if VKWP_SUPPORT_GLFW_SURFACE && defined(VKWP_USE_GLFW_SURFACE)
    [[nodiscard]]
    static SurfaceKHR
    createGLFWSurfaceKHR(std::string             label,
                         const Instance &        instance,
                         GLFWwindow * const      window,
                         const VulkanFunctions * functions = nullptr);
#endif

#if VKWP_SUPPORT_WIN32_SURFACE && defined(_WIN32) && defined(VKWP_USE_WIN32_SURFACE)
    struct OptionalCreateArgsWin32
    {
        const void *                 p_next    = nullptr;
        VkWin32SurfaceCreateFlagsKHR flags     = 0;
        const VulkanFunctions *      functions = nullptr;
    };

    [[nodiscard]]
    static SurfaceKHR
    createWin32SurfaceKHR(std::string                     label,
                          const Instance &                instance,
                          const HINSTANCE                 h_instance,
                          const HWND                      hwnd,
                          const OptionalCreateArgsWin32 & opt_args = {});
#endif

#if VKWP_SUPPORT_WAYLAND_SURFACE && defined(VKWP_USE_WAYLAND_SURFACE)
    struct OptionalCreateArgsWayland
    {
        const void *                   p_next    = nullptr;
        VkWaylandSurfaceCreateFlagsKHR flags     = 0;
        const VulkanFunctions *        functions = nullptr;
    };

    [[nodiscard]]
    static SurfaceKHR
    createWaylandSurfaceKHR(std::string label,
                            const Instance & instance,
                            struct wl_display * const display,
                            struct wl_surface * const surface);

    [[nodiscard]]
    static SurfaceKHR
    createWaylandSurfaceKHR(std::string                       label,
                            const Instance &                  instance,
                            struct wl_display * const         display,
                            struct wl_surface * const         surface,
                            const OptionalCreateArgsWayland & opt_args);
#endif
};

}  // namespace vkwp

#endif
