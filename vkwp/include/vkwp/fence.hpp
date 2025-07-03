#ifndef VKWP_FENCE_HPP
#define VKWP_FENCE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <limits>

namespace vkwp {

///
/// @brief VkFence wrapper.
///
class Fence final : public Resource<VkFence, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkFenceCreateFlags         flags       = VK_FENCE_CREATE_SIGNALED_BIT;
        const DebugUtilsResource * debug_utils = nullptr;
    };

    static Fence
    create(const std::string & label, const Device & device, const OptionalCreateArgs & opt_args = {});

    bool
    wait(const std::uint64_t timeout = (std::numeric_limits<std::uint64_t>::max)()) const;

    void
    resetSignal() const;
};

}  // namespace vkwp

#endif
