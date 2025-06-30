#ifndef VKWP_INSTANCE_HPP
#define VKWP_INSTANCE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <vector>

#if VKWP_SUPPORT_GLFW_SURFACE
struct GLFWwindow;
#endif

#if VKWP_SUPPORT_WIN32_SURFACE
#    include <Windows.h>
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

    [[nodiscard]]
    static std::vector<VkExtensionProperties>
    enumerateInstanceExtensionProperties();

    [[nodiscard]]
    static Instance
    create(const std::string &            label,
           const std::string &            application_name,
           const std::uint32_t            application_version,
           const std::string &            engine_name,
           const std::uint32_t            engine_version,
           const std::uint32_t            api_version,
           const std::vector<std::string> enabled_layer_names,
           const std::vector<std::string> enabled_extension_names,
           const void * const             p_next = nullptr);

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

private:
    [[nodiscard]]
    explicit Instance(const std::string & label);
};

}  // namespace vkwp

#endif
