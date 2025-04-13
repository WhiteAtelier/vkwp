
#include "vkwp/buffer.hpp"

#include "impl/assert.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::Buffer::Buffer() = default;

vkwp::Buffer::Buffer(const std::string & label)
    : Resource{ label }
{}

VkMemoryRequirements
vkwp::Buffer::getMemoryRequirements() const
{
    VKWP_ASSERTION(*this);
    VkMemoryRequirements ret;
    vkGetBufferMemoryRequirements(this->_getParent(), this->_ref(), &ret);
    return ret;
}

void
vkwp::Buffer::bindMemory(const VkDeviceMemory & device_memory, const VkDeviceSize offset) const
{
    VKWP_ASSERTION(device_memory != nullptr);
    VulkanError::check(  //
        vkBindBufferMemory(this->_getParent(), this->_ref(), device_memory, offset),
        "Failed to vkBindImageMemory().");
}
