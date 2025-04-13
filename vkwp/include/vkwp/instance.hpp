#ifndef VKWP_INSTANCE_HPP
#define VKWP_INSTANCE_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

#include <vector>

struct GLFWwindow;

namespace vkwp {

///
/// @brief VkInstance wrapper.
///
class Instance final : public Resource<VkInstance, void, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit Instance();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit Instance(const std::string & label);

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

    [[nodiscard]]
    SurfaceKHR
    createSurfaceKHR(const std::string & label, GLFWwindow * const window) const;
};

}  // namespace vkwp

#endif
