#include "vkwp/command_buffer.hpp"

#include "impl/assert.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::CommandBuffer::CommandBuffer() = default;

vkwp::CommandBuffer::CommandBuffer(const std::string & label)
    : Resource{ label }
{}

void
vkwp::CommandBuffer::beginCommandBuffer(const VkCommandBufferUsageFlags flags, const void * const p_next) const
{
    this->_beginCommandBuffer(nullptr, flags, p_next);
}

void
vkwp::CommandBuffer::beginCommandBuffer(const VkCommandBufferInheritanceInfo & inheritance_info,
                                               const VkCommandBufferUsageFlags        flags,
                                               const void * const                     p_next) const
{
    this->_beginCommandBuffer(&inheritance_info, flags, p_next);
}

void
vkwp::CommandBuffer::_beginCommandBuffer(const VkCommandBufferInheritanceInfo * inheritance_info,
                                                const VkCommandBufferUsageFlags        flags,
                                                const void * const                     p_next) const
{
    const VkCommandBufferBeginInfo bi{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = flags,
        .pInheritanceInfo = nullptr,
    };
    VulkanError::check(  //
        vkBeginCommandBuffer(this->_ref(), &bi),
        "Failed to vkBeginCommandBuffer().");
}

void
vkwp::CommandBuffer::endCommandBuffer() const
{
    VulkanError::check(  //
        vkEndCommandBuffer(this->_ref()),
        "Failed to vkEndCommandBuffer().");
}
