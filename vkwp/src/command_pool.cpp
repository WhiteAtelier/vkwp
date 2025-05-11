#include "vkwp/command_pool.hpp"

#include "impl/assert.hpp"
#include "vkwp/command_buffer.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/vulkan_error.hpp"

#include <optional>

vkwp::CommandPool::CommandPool() = default;

vkwp::CommandPool::CommandPool(const std::string & label)
    : Resource{ label }
{}

std::vector<vkwp::CommandBuffer>
vkwp::CommandPool::allocateCommandBuffers(const std::string &        label,
                                          const VkCommandBufferLevel level,
                                          const std::uint32_t        command_buffer_count,
                                          const void * const         p_next) const
{
    std::vector<CommandBuffer> ret;
    ret.reserve(command_buffer_count);
    for (std::uint32_t i = 0; i < command_buffer_count; ++i)
    {
        ret.emplace_back(label + std::to_string(i));
    }

    std::vector<VkCommandBuffer>      cmdbufs(command_buffer_count, nullptr);
    const VkCommandBufferAllocateInfo ai{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = p_next,
        .commandPool        = this->_ref(),
        .level              = level,
        .commandBufferCount = command_buffer_count,
    };

    {
        // lock は destructor が noexcept 保証できないので,
        // reset() しきってから unlock (scope out) する.
        LockGuardOpt lock{ this->mutex_.get() };

        VulkanError::check(  //
            vkAllocateCommandBuffers(this->_getParent(), &ai, cmdbufs.data()),
            "Failed to vkAllocateCommandBuffers().");

        for (std::size_t i = 0; i < cmdbufs.size(); ++i)
        {
            *ret[i].reset(this->_getParent(), this->_ref()) = cmdbufs[i];
            ret[i].mutex_                                   = this->mutex_.get();
        }
    }

    return ret;
}

vkwp::CommandBuffer
vkwp::CommandPool::allocateCommandBuffer(const std::string &        label,
                                         const VkCommandBufferLevel level,
                                         const void * const         p_next) const
{
    CommandBuffer                     ret{ label };
    const VkCommandBufferAllocateInfo ai{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = p_next,
        .commandPool        = this->_ref(),
        .level              = level,
        .commandBufferCount = 1,
    };

    // lock は destructor が noexcept 保証できないので,
    // reset() しきってから unlock (scope out) する.
    LockGuardOpt lock{ this->mutex_.get() };

    VulkanError::check(  //
        vkAllocateCommandBuffers(this->_getParent(), &ai, ret.reset(this->_getParent(), this->_ref())),
        "Failed to vkAllocateCommandBuffers(). (single allocation)");
    ret.mutex_ = this->mutex_.get();

    return ret;
}

vkwp::CommandBuffer
vkwp::CommandPool::allocatePrimaryCommandBuffer(const std::string & label, const void * const p_next) const
{
    return this->allocateCommandBuffer(label, VK_COMMAND_BUFFER_LEVEL_PRIMARY, p_next);
}
