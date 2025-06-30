#include "vkwp/instance.hpp"

#include "impl/assert.hpp"
#include "vkwp/physical_device.hpp"
#include "vkwp/surface_khr.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::Instance::Instance() = default;

vkwp::Instance::Instance(const std::string & label)
    : Resource{ label }
{}

std::vector<VkExtensionProperties>
vkwp::Instance::enumerateInstanceExtensionProperties()
{
    std::uint32_t count = 0;
    VulkanError::check(  //
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr),
        "Failed to vkEnumerateInstanceExtensionProperties(). (array size.)");
    std::vector<VkExtensionProperties> ret(count);
    VulkanError::check(  //
        vkEnumerateInstanceExtensionProperties(nullptr, &count, ret.data()),
        "Failed to vkEnumerateInstanceExtensionProperties(). (array data.)");
    return ret;
}

/* STATIC */
vkwp::Instance
vkwp::Instance::create(const std::string &            label,
                       const std::string &            application_name,
                       const std::uint32_t            application_version,
                       const std::string &            engine_name,
                       const std::uint32_t            engine_version,
                       const std::uint32_t            api_version,
                       const std::vector<std::string> _enabled_layer_names,
                       const std::vector<std::string> _enabled_extension_names,
                       const void * const             p_next)
{
    Instance                  ret{ label };
    std::vector<const char *> enabled_layer_names;
    std::vector<const char *> enabled_extension_names;
    enabled_layer_names.reserve(_enabled_layer_names.size());
    enabled_extension_names.reserve(_enabled_extension_names.size());
    for (const auto & name : _enabled_layer_names)
    {
        enabled_layer_names.emplace_back(name.c_str());
    }
    for (const auto & name : _enabled_extension_names)
    {
        enabled_extension_names.emplace_back(name.c_str());
    }

    const VkApplicationInfo ai{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = application_name.c_str(),
        .applicationVersion = application_version,
        .pEngineName        = engine_name.c_str(),
        .engineVersion      = engine_version,
        .apiVersion         = api_version,
    };
    const VkInstanceCreateInfo ci{
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = p_next,
        .flags                   = 0,
        .pApplicationInfo        = &ai,
        .enabledLayerCount       = static_cast<std::uint32_t>(enabled_layer_names.size()),
        .ppEnabledLayerNames     = enabled_layer_names.data(),
        .enabledExtensionCount   = static_cast<std::uint32_t>(enabled_extension_names.size()),
        .ppEnabledExtensionNames = enabled_extension_names.data(),
    };

    VulkanError::check(  //
        vkCreateInstance(&ci, nullptr, ret.reset()),
        "Failed to vkCreateInstance().");

    return ret;
}

#if VKWP_SUPPORT_GLFW_SURFACE
#    include <GLFW/glfw3.h>
vkwp::SurfaceKHR
vkwp::Instance::createGLFWSurfaceKHR(const std::string & label, GLFWwindow * const window) const
{
    SurfaceKHR ret{ label };
    VulkanError::check(  //
        glfwCreateWindowSurface(this->_ref(), window, nullptr, ret.reset(this->_ref())),
        "Failed to glfwCreateWindowSurface().");
    return ret;
}
#endif

#if VKWP_SUPPORT_WIN32_SURFACE
vkwp::SurfaceKHR
vkwp::Instance::createWin32SurfaceKHR(const std::string & label, const HINSTANCE h_instance, const HWND hwnd) const
{
    SurfaceKHR                  ret{ label };
    VkWin32SurfaceCreateInfoKHR ci{
        .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext     = nullptr,
        .flags     = 0,
        .hinstance = h_instance,
        .hwnd      = hwnd,
    };
    VulkanError::check(  //
        vkCreateWin32SurfaceKHR(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateWin32SurfaceKHR().");
    return ret;
}
#endif
