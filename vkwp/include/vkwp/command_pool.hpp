#ifndef VKWP_COMMAND_POOL_HPP
#define VKWP_COMMAND_POOL_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"
//
#include "debug_utils_resource.hpp"

#include <memory>
#include <vector>

namespace vkwp {

///
/// @brief VkCommandPool wrapper.
///
class CommandPool final : public Resource<VkCommandPool, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const VkCommandPoolCreateFlags flags      = 0;  ///< もしくは VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        const void *                   p_next     = nullptr;
        const DebugUtilsResource *     debug_util = nullptr;
    };

    [[nodiscard]]
    static CommandPool
    create(const std::string &        label,
           const Device &             device,
           const std::uint32_t        queue_family_index,
           const OptionalCreateArgs & opt_args = {});

    [[nodiscard]]
    static CommandPool
    create(const std::string &        label,
           const Device &             device,
           const std::uint32_t        queue_family_index,
           std::unique_ptr<Mutex> &&  mutex,
           const OptionalCreateArgs & opt_args = {});

    VkCommandPoolCreateFlags
    getCreateFlags() const noexcept;

private:
    friend CommandBuffer;
    VkCommandPoolCreateFlags create_flags_;
    std::unique_ptr<Mutex>   mutex_;
};

}  // namespace vkwp

#endif
