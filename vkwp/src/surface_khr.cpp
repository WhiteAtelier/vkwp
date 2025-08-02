#include "vkwp/surface_khr.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/instance.hpp"
#include "vkwp/vulkan_error.hpp"

#if VKWP_SUPPORT_GLFW_SURFACE
#    include <GLFW/glfw3.h>
#endif

#if VKWP_SUPPORT_WIN32_SURFACE
#    include <Windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkSurfaceKHR, VkInstance>(VkSurfaceKHR i, const std::tuple<VkInstance> & args) noexcept
{
    vkDestroySurfaceKHR(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineCache class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
#if VKWP_SUPPORT_GLFW_SURFACE
vkwp::SurfaceKHR
vkwp::SurfaceKHR::createGLFWSurfaceKHR(const std::string & label,
                                       const Instance &    instance,
                                       GLFWwindow * const  window)
{
    SurfaceKHR ret{ label };
    VulkanError::check(  //
        glfwCreateWindowSurface(instance, window, nullptr, ret._reset(instance)),
        "Failed to glfwCreateWindowSurface().");
    return ret;
}
#endif

#if VKWP_SUPPORT_WIN32_SURFACE
/* STATIC */
vkwp::SurfaceKHR
vkwp::SurfaceKHR::createWin32SurfaceKHR(const std::string &             label,
                                        const Instance &                instance,
                                        const HINSTANCE                 h_instance,
                                        const HWND                      hwnd,
                                        const OptionalCreateArgsWin32 & opt_args)
{
    SurfaceKHR                  ret{ label };
    VkWin32SurfaceCreateInfoKHR ci{
        .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext     = opt_args.p_next,
        .flags     = opt_args.flags,
        .hinstance = h_instance,
        .hwnd      = hwnd,
    };
    VulkanError::check(  //
        vkCreateWin32SurfaceKHR(instance, &ci, nullptr, ret._reset(instance)),
        "Failed to vkCreateWin32SurfaceKHR().");
    return ret;
}
#endif