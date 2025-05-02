#ifndef VKWP_COMMAND_POOL_HPP
#define VKWP_COMMAND_POOL_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <vector>

namespace vkwp {

///
/// @brief VkCommandPool wrapper.
///
class CommandPool final : public Resource<VkCommandPool, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit CommandPool();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit CommandPool(const std::string & label);

    [[nodiscard]]
    std::vector<CommandBuffer>
    allocateCommandBuffers(const std::string &        label,
                           const VkCommandBufferLevel level,
                           const std::uint32_t        command_buffer_count,
                           const void * const         p_next = nullptr) const;

    [[nodiscard]]
    CommandBuffer
    allocateCommandBuffer(const std::string &        label,
                          const VkCommandBufferLevel level,
                          const void * const         p_next = nullptr) const;

    [[nodiscard]]
    CommandBuffer
    allocatePrimaryCommandBuffer(const std::string & label, const void * const p_next = nullptr) const;
};

}  // namespace vkwp

#endif
