#ifndef VKWP_COMMAND_BUFFER_HPP
#define VKWP_COMMAND_BUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <vector>

namespace vkwp {

///
/// @brief VkCommandBuffer wrapper.
///
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCommandBuffer.html
///
class CommandBuffer final : public Resource<VkCommandBuffer, VkDevice, VkCommandPool>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit CommandBuffer();

    CommandBuffer(const CommandBuffer &) = delete;
    CommandBuffer(CommandBuffer &&) noexcept;

    ~CommandBuffer() noexcept;

    CommandBuffer &
    operator=(const CommandBuffer &)
        = delete;

    CommandBuffer &
    operator=(CommandBuffer &&) & noexcept;

    struct OptionalAllocateArgs
    {
        const void *               p_next      = nullptr;
        const DebugUtilsResource * debug_utils = nullptr;
    };

    [[nodiscard]]
    static std::vector<CommandBuffer>
    allocateCommandBuffers(const std::string &          label,
                           const CommandPool &          command_pool,
                           const VkCommandBufferLevel   level,
                           const std::uint32_t          command_buffer_count,
                           const OptionalAllocateArgs & opt_args = {});

    [[nodiscard]]
    static CommandBuffer
    allocateCommandBuffer(const std::string &          label,
                          const CommandPool &          command_pool,
                          const VkCommandBufferLevel   level,
                          const OptionalAllocateArgs & opt_args = {});

    [[nodiscard]]
    static CommandBuffer
    allocatePrimaryCommandBuffer(const std::string &          label,
                                 const CommandPool &          command_pool,
                                 const OptionalAllocateArgs & opt_args = {});

    void
    beginCommandBuffer(const VkCommandBufferInheritanceInfo & inheritance_info,
                       const VkCommandBufferUsageFlags        flags  = 0,
                       const void * const                     p_next = nullptr) const;

    void
    beginCommandBuffer(const VkCommandBufferUsageFlags flags = 0, const void * const p_next = nullptr) const;

    void
    endCommandBuffer() const;

    /// @brief Lock this queue for synchronized.
    ///
    /// Should not use this function directly, but use with std::lock_guard, std::unique_lock, or etc...
    void
    lock() const;

    /// @brief Try to lock this queue for synchronized.
    ///
    /// Should not use this function directly, but use with std::lock_guard, std::unique_lock, or etc...
    bool
    try_lock() const;

    /// @brief Unlock this queue for synchronized.
    void
    unlock() const;

private:
    void
    _beginCommandBuffer(const VkCommandBufferInheritanceInfo * inheritance_info,
                        const VkCommandBufferUsageFlags        flags,
                        const void * const                     p_next) const;

    [[nodiscard]]
    explicit CommandBuffer(const std::string & label);

    Mutex * mutex_;
};

}  // namespace vkwp

#endif
