#ifndef VKWP_SEMAPHORE_HPP
#define VKWP_SEMAPHORE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>

namespace vkwp {

///
/// @brief VkSemaphore wrapper.
///
class Semaphore final : public Resource<VkSemaphore, PFN_vkDestroySemaphore, VkDevice>
{
public:
    using Resource<VkSemaphore, PFN_vkDestroySemaphore, VkDevice>::Resource;

    [[nodiscard]]
    static Semaphore
    importExternal(std::string label, const Device & device, const VkSemaphore external);

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkSemaphoreCreateFlags     flags       = 0;
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    static Semaphore
    create(std::string label, const Device & device);

    static Semaphore
    create(std::string label, const Device & device, const OptionalCreateArgs & opt_args);
};

}  // namespace vkwp

#endif
