#ifndef VKWP_PIPELINE_LAYOUT_HPP
#define VKWP_PIPELINE_LAYOUT_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string_view>
#include <type_traits>
#include <vector>

namespace vkwp {

///
/// @brief VkPipelineLayout wrapper.
///
class PipelineLayout final : public Resource<VkPipelineLayout, PFN_vkDestroyPipelineLayout, VkDevice>
{
public:
    using Resource<VkPipelineLayout, PFN_vkDestroyPipelineLayout, VkDevice>::Resource;

    struct OptionalCreateArgs
    {
        const void *                p_next      = nullptr;
        VkPipelineLayoutCreateFlags flags       = 0;
        const DebugUtilsResource *  debug_utils = nullptr;
        const VulkanFunctions *     functions   = nullptr;
    };

    static PipelineLayout
    create(const std::string_view                                                 label,
           const Device &                                                         device,
           const std::vector<std::reference_wrapper<const DescriptorSetLayout>> & set_layouts,
           const std::vector<VkPushConstantRange> &                               push_constant_ranges);

    static PipelineLayout
    create(const std::string_view                                                 label,
           const Device &                                                         device,
           const std::vector<std::reference_wrapper<const DescriptorSetLayout>> & set_layouts,
           const std::vector<VkPushConstantRange> &                               push_constant_ranges,
           const OptionalCreateArgs &                                             opt_args);
};

}  // namespace vkwp

#endif
