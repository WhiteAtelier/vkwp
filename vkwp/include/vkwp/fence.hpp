#ifndef VKWP_FENCE_HPP
#define VKWP_FENCE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <limits>
#include <string_view>

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
    create(const std::string_view label, const Device & device);

    static Fence
    create(const std::string_view label, const Device & device, const OptionalCreateArgs & opt_args);

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
