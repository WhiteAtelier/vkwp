// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_FENCE_HPP
#define VKWP_FENCE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <limits>
#include <string>

namespace vkwp {

///
/// @brief VkFence wrapper.
///
class Fence final : public Resource<VkFence, PFN_vkDestroyFence, VkDevice>
{
public:
    using Resource<VkFence, PFN_vkDestroyFence, VkDevice>::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkFenceCreateFlags         flags       = VK_FENCE_CREATE_SIGNALED_BIT;
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    static Fence
    create(std::string label, const Device & device);

    static Fence
    create(std::string label, const Device & device, const OptionalCreateArgs & opt_args);

    [[nodiscard]]
    static Fence
    importExternal(std::string                   label,
                   const Device &                device,
                   const VkFence                 external,
                   const VulkanFunctions * const functions = nullptr);

    bool
    wait(const std::uint64_t timeout = (std::numeric_limits<std::uint64_t>::max)()) const;

    void
    resetSignal() const;

private:
    PFN_vkWaitForFences _vkWaitForFences = nullptr;
    PFN_vkResetFences   _vkResetFences   = nullptr;
};

}  // namespace vkwp

#endif
