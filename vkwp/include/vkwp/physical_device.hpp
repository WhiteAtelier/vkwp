#ifndef VKWP_PHYSICAL_DEVICE_HPP
#define VKWP_PHYSICAL_DEVICE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace vkwp {

///
/// @brief VkPhysicalDevice wrapper.
///
class PhysicalDevice final : public Resource<VkPhysicalDevice>
{
public:
    using Resource<VkPhysicalDevice>::Resource;

    static std::vector<PhysicalDevice>
    enumeratePhysicalDevices(const VkInstance instance);

    [[nodiscard]]
    const std::vector<VkQueueFamilyProperties> &
    getQueueFamilyProperties() const;

    [[nodiscard]]
    const VkPhysicalDeviceProperties &
    getProperties() const noexcept;

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
    explicit PhysicalDevice(const std::string & label, VkPhysicalDeviceProperties && pd_properties);

    VkPhysicalDeviceProperties                                  properties_{};
    mutable std::optional<std::vector<VkQueueFamilyProperties>> queue_family_properties_cache_;
    mutable std::optional<VkPhysicalDeviceMemoryProperties>     memory_properties_;
};

}  // namespace vkwp

#endif
