#ifndef VKWP_DESCRIPTOR_SET_LAYOUT_HPP
#define VKWP_DESCRIPTOR_SET_LAYOUT_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkDescriptorSetLayout wrapper.
///
class DescriptorSetLayout final : public Resource<VkDescriptorSetLayout, PFN_vkDestroyDescriptorSetLayout, VkDevice>
{
public:
    using Resource<VkDescriptorSetLayout, PFN_vkDestroyDescriptorSetLayout, VkDevice>::Resource;

    [[nodiscard]]
    static DescriptorSetLayout
    importExternal(std::string label, const Device & device, const VkDescriptorSetLayout external);

    struct OptionalCreateArgs
    {
        const void *                     p_next      = nullptr;
        VkDescriptorSetLayoutCreateFlags flags       = 0;
        const DebugUtilsResource *       debug_utils = nullptr;
        const VulkanFunctions *          functions   = nullptr;
    };

    static DescriptorSetLayout
    create(std::string label, const Device & device, const std::vector<VkDescriptorSetLayoutBinding> & bindings);

    static DescriptorSetLayout
    create(std::string                                       label,
           const Device &                                    device,
           const std::vector<VkDescriptorSetLayoutBinding> & bindings,
           const OptionalCreateArgs &                        opt_args);
};

}  // namespace vkwp

#endif
