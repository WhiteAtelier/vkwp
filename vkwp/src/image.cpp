#include "vkwp/image.hpp"

#include "vkwp/vulkan_error.hpp"

vkwp::Image::Image() = default;

vkwp::Image::Image(const std::string & label)
    : Resource{ label }
{}

VkMemoryRequirements
vkwp::Image::getMemoryRequirements() const
{
    VkMemoryRequirements ret;
    vkGetImageMemoryRequirements(this->_getParent(), this->_ref(), &ret);
    return ret;
}

void
vkwp::Image::bindMemory(const VkDeviceMemory & device_memory, const VkDeviceSize offset) const
{
    VulkanError::check(  //
        vkBindImageMemory(this->_getParent(), this->_ref(), device_memory, offset),
        "Failed to vkBindImageMemory().");
}
