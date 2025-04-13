#ifndef VKWP_INSTANCE_HPP
#define VKWP_INSTANCE_HPP

#include "fwd.hpp"
#include "resource.hpp"

// befor include vulkan.
#if VKWP_SUPPORT_WIN32_SURFACE
#    include <Windows.h>
#    define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

#include <vector>

#if VKWP_SUPPORT_GLFW_SURFACE
struct GLFWwindow;
#endif

namespace vkwp {

///
/// @brief VkInstance wrapper.
///
class Instance final : public Resource<VkInstance, void, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit Instance();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit Instance(const std::string & label);

    [[nodiscard]]
    static std::vector<VkExtensionProperties>
    enumerateInstanceExtensionProperties();

    [[nodiscard]]
    static Instance
    createInstance(const std::string &            label,
                   const std::string &            application_name,
                   const std::uint32_t            application_version,
                   const std::string &            engine_name,
                   const std::uint32_t            engine_version,
                   const std::uint32_t            api_version,
                   const std::vector<std::string> enabled_layer_names,
                   const std::vector<std::string> enabled_extension_names,
                   const void * const             p_next = nullptr);

    [[nodiscard]]
    std::vector<PhysicalDevice>
    enumeratePhysicalDevices() const;

#if VKWP_SUPPORT_GLFW_SURFACE
    [[nodiscard]]
    SurfaceKHR
    createGLFWSurfaceKHR(const std::string & label, GLFWwindow * const window) const;
#endif

#if VKWP_SUPPORT_WIN32_SURFACE
    [[nodiscard]]
    SurfaceKHR
    createWin32SurfaceKHR(const std::string & label, const HINSTANCE h_instance, const HWND hwnd) const;
#endif
};

}  // namespace vkwp

#endif
