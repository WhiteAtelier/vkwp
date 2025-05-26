#include "vkwp/command_buffer.hpp"

#include "impl/assert.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::CommandBuffer::CommandBuffer() = default;

vkwp::CommandBuffer::CommandBuffer(const std::string & label)
    : Resource{ label }
    , mutex_{ nullptr }
{}

vkwp::CommandBuffer::CommandBuffer(CommandBuffer &&) noexcept = default;

vkwp::CommandBuffer::~CommandBuffer() noexcept
{
    LockGuardOpt lock{ this->mutex_ };
    this->reset();
}

vkwp::CommandBuffer &
vkwp::CommandBuffer::operator=(CommandBuffer &&) & noexcept
    = default;

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

void
vkwp::CommandBuffer::lock() const
{
    if (this->mutex_ != nullptr)
    {
        this->mutex_->lock();
    }
}

bool
vkwp::CommandBuffer::try_lock() const
{
    if (this->mutex_ != nullptr)
    {
        return this->mutex_->tryLock();
    }
    else
    {
        return true;
    }
}

void
vkwp::CommandBuffer::unlock() const
{
    if (this->mutex_ != nullptr)
    {
        this->mutex_->unlock();
    }
}
