#ifndef VKWP_COMMAND_BUFFER_HPP
#define VKWP_COMMAND_BUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

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
    [[nodiscard]] explicit CommandBuffer();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit CommandBuffer(const std::string & label);

    void
    beginCommandBuffer(const VkCommandBufferInheritanceInfo & inheritance_info,
                       const VkCommandBufferUsageFlags        flags  = 0,
                       const void * const                     p_next = nullptr) const;

    void
    beginCommandBuffer(const VkCommandBufferUsageFlags flags = 0, const void * const p_next = nullptr) const;

    void
    endCommandBuffer() const;

private:
    void
    _beginCommandBuffer(const VkCommandBufferInheritanceInfo * inheritance_info,
                        const VkCommandBufferUsageFlags        flags,
                        const void * const                     p_next) const;
};

}  // namespace vkwp

#endif
