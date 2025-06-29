#ifndef VKWP_DESCRIPTOR_POOL_HPP
#define VKWP_DESCRIPTOR_POOL_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"
//
#include "debug_utils_resource.hpp"

#include <vector>

namespace vkwp {

///
/// @brief VkDescriptorPool wrapper.
///
class DescriptorPool final : public Resource<VkDescriptorPool, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit DescriptorPool();

    /// @brief Constructor with label name.
    explicit DescriptorPool(const std::string & label);

    void
    setDebugUtilsResource(const DebugUtilsResource & debug_utils_resource);

    [[nodiscard]]
    std::vector<DescriptorSet>
    allocateDescriptorSets(const std::string &                        label,
                           const std::vector<VkDescriptorSetLayout> & set_layouts,
                           const void * const                         p_next = nullptr) const;

    [[nodiscard]]
    DescriptorSet
    allocateDescriptorSet(const std::string &           label,
                          const VkDescriptorSetLayout & set_layouts,
                          const void * const            p_next = nullptr) const;

private:
    DebugUtilsResource debug_utils_resource_;
};

}  // namespace vkwp

#endif
