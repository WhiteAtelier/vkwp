#ifndef VKWP_DESCRIPTOR_SET_LAYOUT_HPP
#define VKWP_DESCRIPTOR_SET_LAYOUT_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <string_view>
#include <vector>

namespace vkwp {

///
/// @brief VkDescriptorSetLayout wrapper.
///
class DescriptorSetLayout final : public Resource<VkDescriptorSetLayout, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *                     p_next      = nullptr;
        VkDescriptorSetLayoutCreateFlags flags       = 0;
        const DebugUtilsResource *       debug_utils = nullptr;
    };

    static DescriptorSetLayout
    create(const std::string_view                            label,
           const Device &                                    device,
           const std::vector<VkDescriptorSetLayoutBinding> & bindings);

    static DescriptorSetLayout
    create(const std::string_view                            label,
           const Device &                                    device,
           const std::vector<VkDescriptorSetLayoutBinding> & bindings,
           const OptionalCreateArgs &                        opt_args);
};

}  // namespace vkwp

#endif
