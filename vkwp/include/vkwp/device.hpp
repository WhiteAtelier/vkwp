
#ifndef VKWP_DEVICE_HPP
#define VKWP_DEVICE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <memory>
#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkDevice wrapper.
///
class Device final : public Resource<VkDevice, PFN_vkDestroyDevice>
{
public:
    using Resource<VkDevice, PFN_vkDestroyDevice>::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    static Device
    create(std::string                                  label,
           const VkPhysicalDevice                       physical_device,
           const std::vector<VkDeviceQueueCreateInfo> & queue_create_infos,
           const std::vector<std::string> &             enabled_layer_names,
           const std::vector<std::string> &             enabled_extension_names,
           const VkPhysicalDeviceFeatures * const       enabled_features);

    static Device
    create(std::string                                  label,
           const VkPhysicalDevice                       physical_device,
           const std::vector<VkDeviceQueueCreateInfo> & queue_create_infos,
           const std::vector<std::string> &             enabled_layer_names,
           const std::vector<std::string> &             enabled_extension_names,
           const VkPhysicalDeviceFeatures * const       enabled_features,
           const OptionalCreateArgs &                   opt_args);

    void
    waitIdle() const;

    struct DeviceQueueGetArgs
    {
        const DebugUtilsResource * debug_utils = nullptr;
    };

    [[nodiscard]]
    Queue
    getDeviceQueue(const std::uint32_t queue_family_index,  //
                   const std::uint32_t queue_index) const;

    [[nodiscard]]
    Queue
    getDeviceQueue(const std::uint32_t       queue_family_index,
                   const std::uint32_t       queue_index,
                   std::unique_ptr<Mutex> && mutex) const;

    [[nodiscard]]
    Queue
    getDeviceQueue(const std::uint32_t        queue_family_index,
                   const std::uint32_t        queue_index,
                   std::unique_ptr<Mutex> &&  mutex,
                   const DeviceQueueGetArgs & opt_args) const;

private:
    PFN_vkDeviceWaitIdle _vkDeviceWaitIdle = nullptr;
    PFN_vkGetDeviceQueue _vkGetDeviceQueue = nullptr;
};

}  // namespace vkwp

#endif
