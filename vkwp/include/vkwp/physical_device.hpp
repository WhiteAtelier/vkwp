#ifndef VKWP_PHYSICAL_DEVICE_HPP
#define VKWP_PHYSICAL_DEVICE_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace vkwp {

///
/// @brief VkPhysicalDevice wrapper.
///
class PhysicalDevice final : public Resource<VkPhysicalDevice, void, void>
{
public:
    /// @brief Default Constructor
    explicit PhysicalDevice();

    /// @brief Constructor with label name.
    explicit PhysicalDevice(const std::string & label);

    [[nodiscard]]
    const std::vector<VkQueueFamilyProperties> &
    getQueueFamilyProperties() const;

    [[nodiscard]]
    const VkPhysicalDeviceProperties &
    getProperties() const;

    [[nodiscard]]
    const VkPhysicalDeviceMemoryProperties &
    getMemoryProperties() const;

    [[nodiscard]]
    std::vector<VkExtensionProperties>
    enumerateDeviceExtensionProperties() const;

    [[nodiscard]]
    VkFormatProperties
    getFormatProperties(const VkFormat format) const;

    [[nodiscard]]
    Device
    createDevice(const std::string &                          label,
                 const std::vector<VkDeviceQueueCreateInfo> & queue_create_infos,
                 const std::vector<std::string> &             enabled_layer_names,
                 const std::vector<std::string> &             enabled_extension_names,
                 const VkPhysicalDeviceFeatures &             enabled_features,
                 const void *                                 p_next = nullptr);

    [[nodiscard]]
    std::vector<VkSurfaceFormatKHR>
    getSurfaceFormats(const VkSurfaceKHR & surface) const;

    [[nodiscard]]
    VkSurfaceCapabilitiesKHR
    getSurfaceCapabilities(const VkSurfaceKHR & surface) const;

    [[nodiscard]]
    std::pair<bool, std::uint32_t>
    getMemoryTypeIndex(const VkMemoryPropertyFlags memory_property_flags, const std::uint32_t memory_type_bits) const;

    [[nodiscard]]
    std::set<std::uint32_t>
    listQueueFamilyIndexes(const VkQueueFlags & queue_flags) const;

    [[nodiscard]]
    bool
    isSurfaceSupported(const std::uint32_t queue_index, const VkSurfaceKHR & surface) const;

private:
    mutable std::optional<std::vector<VkQueueFamilyProperties>> queue_family_properties_cache_;
    mutable std::optional<VkPhysicalDeviceProperties>           properties_;
    mutable std::optional<VkPhysicalDeviceMemoryProperties>     memory_properties_;
};

}  // namespace vkwp

#endif
