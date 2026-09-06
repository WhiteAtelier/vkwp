#ifndef VKWP_DESCRIPTOR_SET_HPP
#define VKWP_DESCRIPTOR_SET_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <vector>

namespace vkwp {

struct DescriptorSetAllocateInfo
{
    std::string                 label;
    const DescriptorSetLayout & descriptor_set_layout;
};

///
/// @brief VkDescriptorSet wrapper.
///
class DescriptorSet final : public Resource<VkDescriptorSet, PFN_vkFreeDescriptorSets, VkDevice, VkDescriptorPool>
{
public:
    using Resource<VkDescriptorSet, PFN_vkFreeDescriptorSets, VkDevice, VkDescriptorPool>::Resource;

    [[nodiscard]]
    static DescriptorSet
    importExternal(std::string            label,
                   const Device &         device,
                   const DescriptorPool & descriptor_pool,
                   const VkDescriptorSet  external);

    struct OptionalAllocateArgs
    {
        const void *               p_next      = nullptr;
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    static std::vector<DescriptorSet>
    allocate(const DescriptorPool &                 descriptor_pool,  //
             std::vector<DescriptorSetAllocateInfo> allocate_info_list);

    static std::vector<DescriptorSet>
    allocate(const DescriptorPool &                 descriptor_pool,
             std::vector<DescriptorSetAllocateInfo> allocate_info_list,
             const OptionalAllocateArgs &           opt_args);
};

}  // namespace vkwp

#endif
