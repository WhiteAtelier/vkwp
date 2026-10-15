// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_DESCRIPTOR_POOL_HPP
#define VKWP_DESCRIPTOR_POOL_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkDescriptorPool wrapper.
///
class DescriptorPool final : public Resource<VkDescriptorPool, PFN_vkDestroyDescriptorPool, VkDevice>
{
public:
    using Resource<VkDescriptorPool, PFN_vkDestroyDescriptorPool, VkDevice>::Resource;

    [[nodiscard]]
    static DescriptorPool
    importExternal(std::string                       label,
                   const Device &                    device,
                   const VkDescriptorPool            external,
                   const VkDescriptorPoolCreateFlags create_flags);

    struct OptionalCreateArgs
    {
        const void *                p_next      = nullptr;
        VkDescriptorPoolCreateFlags flags       = 0;  // または VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        const DebugUtilsResource *  debug_utils = nullptr;
        const VulkanFunctions *     functions   = nullptr;
    };

    [[nodiscard]]
    static DescriptorPool
    create(std::string                               label,
           const Device &                            device,
           const std::uint32_t                       max_sets,
           const std::vector<VkDescriptorPoolSize> & pool_sizes);

    [[nodiscard]]
    static DescriptorPool
    create(std::string                               label,
           const Device &                            device,
           const std::uint32_t                       max_sets,
           const std::vector<VkDescriptorPoolSize> & pool_sizes,
           const OptionalCreateArgs &                opt_args);

    [[nodiscard]]
    VkDescriptorPoolCreateFlags
    getCreateFlags() const noexcept;

private:
    friend DescriptorSet;
    VkDescriptorPoolCreateFlags create_flags_;
};

}  // namespace vkwp

#endif
