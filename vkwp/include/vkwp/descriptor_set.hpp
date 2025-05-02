#ifndef VKWP_DESCRIPTOR_SET_HPP
#define VKWP_DESCRIPTOR_SET_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkDescriptorSet wrapper.
///
class DescriptorSet final : public Resource<VkDescriptorSet, VkDevice, VkDescriptorPool>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit DescriptorSet();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit DescriptorSet(const std::string & label);
};

}  // namespace vkwp

#endif
