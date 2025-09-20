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
class DescriptorSet final : public Resource<VkDescriptorSet, VkDevice, VkDescriptorPool>
{
public:
    using Resource::Resource;

    struct OptionalAllocateArgs
    {
        const void *               p_next      = nullptr;
        const DebugUtilsResource * debug_utils = nullptr;
    };

    static const OptionalAllocateArgs default_opt_args;

    static std::vector<DescriptorSet>
    allocate(const DescriptorPool &                         descriptor_pool,
             const std::vector<DescriptorSetAllocateInfo> & allocate_info_list,
             const OptionalAllocateArgs &                   opt_args = default_opt_args);
};

}  // namespace vkwp

#endif
