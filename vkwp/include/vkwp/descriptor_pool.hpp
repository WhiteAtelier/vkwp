#ifndef VKWP_DESCRIPTOR_POOL_HPP
#define VKWP_DESCRIPTOR_POOL_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <vector>

namespace vkwp {

///
/// @brief VkDescriptorPool wrapper.
///
class DescriptorPool final : public Resource<VkDescriptorPool, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *                p_next      = nullptr;
        VkDescriptorPoolCreateFlags flags       = 0;  // または VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        const DebugUtilsResource *  debug_utils = nullptr;
    };

    static const OptionalCreateArgs default_opt_args;

    [[nodiscard]]
    static DescriptorPool
    create(const std::string &                       label,
           const Device &                            device,
           const std::uint32_t                       max_sets,
           const std::vector<VkDescriptorPoolSize> & pool_sizes,
           const OptionalCreateArgs &                opt_args = default_opt_args);

    [[nodiscard]]
    VkDescriptorPoolCreateFlags
    getCreateFlags() const noexcept;

private:
    friend DescriptorSet;
    VkDescriptorPoolCreateFlags create_flags_;
};

}  // namespace vkwp

#endif
