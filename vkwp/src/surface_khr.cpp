#include "vkwp/surface_khr.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/instance.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

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
vkwp::impl::_delete(VkSurfaceKHR i, PFN_vkDestroySurfaceKHR fn, const std::tuple<VkInstance> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineCache class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
#if VKWP_SUPPORT_GLFW_SURFACE
vkwp::SurfaceKHR
vkwp::SurfaceKHR::createGLFWSurfaceKHR(std::string             label,
                                       const Instance &        instance,
                                       GLFWwindow * const      window,
                                       const VulkanFunctions * functions)
{
    SurfaceKHR   ret{ std::move(label) };
    auto * const destroy_fn = (functions != nullptr && functions->vkDestroySurfaceKHR != nullptr)
                                ? functions->vkDestroySurfaceKHR
                                : vkDestroySurfaceKHR;
    VulkanError::check(  //
        glfwCreateWindowSurface(instance, window, nullptr, ret._reset(destroy_fn, instance)),
        "Failed to glfwCreateWindowSurface().");
    return ret;
}
#endif

#if VKWP_SUPPORT_WIN32_SURFACE
/* STATIC */
vkwp::SurfaceKHR
vkwp::SurfaceKHR::createWin32SurfaceKHR(std::string                     label,
                                        const Instance &                instance,
                                        const HINSTANCE                 h_instance,
                                        const HWND                      hwnd,
                                        const OptionalCreateArgsWin32 & opt_args)
{
    SurfaceKHR                  ret{ std::move(label) };
    VkWin32SurfaceCreateInfoKHR ci{
        .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext     = opt_args.p_next,
        .flags     = opt_args.flags,
        .hinstance = h_instance,
        .hwnd      = hwnd,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateWin32SurfaceKHR != nullptr)
                                ? opt_args.functions->vkCreateWin32SurfaceKHR
                                : vkCreateWin32SurfaceKHR;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroySurfaceKHR != nullptr)
                                ? opt_args.functions->vkDestroySurfaceKHR
                                : vkDestroySurfaceKHR;

    VulkanError::check(  //
        create_fn(instance, &ci, nullptr, ret._reset(destroy_fn, instance)),
        "Failed to vkCreateWin32SurfaceKHR().");
    return ret;
}
#endif
