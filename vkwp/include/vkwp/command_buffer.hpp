#ifndef VKWP_COMMAND_BUFFER_HPP
#define VKWP_COMMAND_BUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

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

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit CommandBuffer(const std::string & label);

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

    friend CommandPool;
    Mutex * mutex_;
};

}  // namespace vkwp

#endif
