#ifndef VKWP_DESCRIPTOR_SET_LAYOUT_HPP
#define VKWP_DESCRIPTOR_SET_LAYOUT_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief VkDescriptorSetLayout wrapper.
///
class DescriptorSetLayout final : public Resource<VkDescriptorSetLayout, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit DescriptorSetLayout();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit DescriptorSetLayout(const std::string & label);
};

}  // namespace vkwp

#endif
