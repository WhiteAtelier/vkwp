#ifndef VKWP_COMMAND_BUFFER_HPP
#define VKWP_COMMAND_BUFFER_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkCommandBuffer wrapper.
///
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCommandBuffer.html
///
class CommandBuffer final : public Resource<VkCommandBuffer, PFN_vkFreeCommandBuffers, VkDevice, VkCommandPool>
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
        const VulkanFunctions *    functions   = nullptr;
    };

    [[nodiscard]]
    static std::vector<CommandBuffer>
    allocateCommandBuffers(const CommandPool &        command_pool,
                           const VkCommandBufferLevel level,
                           std::vector<std::string>   command_buffer_labels);

    [[nodiscard]]
    static std::vector<CommandBuffer>
    allocateCommandBuffers(const CommandPool &          command_pool,
                           const VkCommandBufferLevel   level,
                           std::vector<std::string>     command_buffer_labels,
                           const OptionalAllocateArgs & opt_args);

    [[nodiscard]]
    static CommandBuffer
    allocateCommandBuffer(std::string label, const CommandPool & command_pool, const VkCommandBufferLevel level);

    [[nodiscard]]
    static CommandBuffer
    allocateCommandBuffer(std::string                  label,
                          const CommandPool &          command_pool,
                          const VkCommandBufferLevel   level,
                          const OptionalAllocateArgs & opt_args);

    [[nodiscard]]
    static CommandBuffer
    allocatePrimaryCommandBuffer(std::string         label,  //
                                 const CommandPool & command_pool);

    [[nodiscard]]
    static CommandBuffer
    allocatePrimaryCommandBuffer(std::string                  label,
                                 const CommandPool &          command_pool,
                                 const OptionalAllocateArgs & opt_args);

    [[nodiscard]]
    static CommandBuffer
    importExternal(std::string                   label,
                   const CommandPool &           command_pool,
                   const VkCommandBuffer         external,
                   const VulkanFunctions * const functions = nullptr);

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
    explicit CommandBuffer(std::string && label);

    explicit CommandBuffer(std::string &&        label,
                           const VkCommandBuffer external,
                           const VkDevice        device,
                           const VkCommandPool   command_pool);

    Mutex *                  mutex_;
    PFN_vkBeginCommandBuffer _vkBeginCommandBuffer = nullptr;
    PFN_vkEndCommandBuffer   _vkEndCommandBuffer   = nullptr;
};

}  // namespace vkwp

#endif
