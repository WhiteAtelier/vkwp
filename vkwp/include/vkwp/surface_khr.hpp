#ifndef VKWP_SURFACE_KHR_HPP
#define VKWP_SURFACE_KHR_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#if defined(VKWP_USE_GLFW_SURFACE)
struct GLFWwindow;
#endif

#if defined(_WIN32) && defined(VKWP_USE_WIN32_SURFACE)
#    include <Windows.h>
#endif

namespace vkwp {

///
/// @brief VkSurfaceKHR wrapper.
///
class SurfaceKHR final : public Resource<VkSurfaceKHR, VkInstance>
{
public:
    using Resource::Resource;

#if defined(VKWP_USE_GLFW_SURFACE)
    [[nodiscard]]
    static SurfaceKHR
    createGLFWSurfaceKHR(const std::string & label, const Instance & instance, GLFWwindow * const window);
#endif

#if defined(_WIN32) && defined(VKWP_USE_WIN32_SURFACE)
    struct OptionalCreateArgsWin32
    {
        const void *                 p_next = nullptr;
        VkWin32SurfaceCreateFlagsKHR flags  = 0;
    };

    static const OptionalCreateArgsWin32 default_win32_opt_args;

    [[nodiscard]]
    static SurfaceKHR
    createWin32SurfaceKHR(const std::string &             label,
                          const Instance &                instance,
                          const HINSTANCE                 h_instance,
                          const HWND                      hwnd,
                          const OptionalCreateArgsWin32 & opt_args = default_win32_opt_args);
#endif
};

}  // namespace vkwp

#endif
