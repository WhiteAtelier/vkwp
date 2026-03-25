#ifndef VKWP_COMMAND_POOL_HPP
#define VKWP_COMMAND_POOL_HPP

#include "fwd.hpp"
#include "mutex.hpp"
#include "resource.hpp"
#include "vulkan.hpp"
//
#include "debug_utils_resource.hpp"

#include <cstdint>
#include <memory>
#include <string_view>
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
        VkCommandPoolCreateFlags   flags       = 0;  ///< もしくは VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        const void *               p_next      = nullptr;
        const DebugUtilsResource * debug_utils = nullptr;
    };

    [[nodiscard]]
    static CommandPool
    create(const std::string_view label,  //
           const Device &         device,
           const std::uint32_t    queue_family_index);

    [[nodiscard]]
    static CommandPool
    create(const std::string_view    label,
           const Device &            device,
           const std::uint32_t       queue_family_index,
           std::unique_ptr<Mutex> && mutex);

    [[nodiscard]]
    static CommandPool
    create(const std::string_view     label,
           const Device &             device,
           const std::uint32_t        queue_family_index,
           std::unique_ptr<Mutex> &&  mutex,
           const OptionalCreateArgs & opt_args);

    VkCommandPoolCreateFlags
    getCreateFlags() const noexcept;

    /// @brief vkResetCommandPool - Reset a command pool
    void
    resetCommandPool(const VkCommandPoolResetFlags flags = 0) const;

private:
    friend CommandBuffer;
    VkCommandPoolCreateFlags create_flags_;
    std::unique_ptr<Mutex>   mutex_;
};

}  // namespace vkwp

#endif
