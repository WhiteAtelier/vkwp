#ifndef VKWP_SEMAPHORE_HPP
#define VKWP_SEMAPHORE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkSemaphore wrapper.
///
class Semaphore final : public Resource<VkSemaphore, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkSemaphoreCreateFlags     flags       = 0;
        const DebugUtilsResource * debug_utils = nullptr;
    };

    static Semaphore
    create(const std::string &        label,
           const Device &             device,
           const OptionalCreateArgs & opt_args = OptionalCreateArgs{});
};

}  // namespace vkwp

#endif
