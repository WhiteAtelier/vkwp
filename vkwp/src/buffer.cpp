
#include "vkwp/buffer.hpp"

#include "impl/assert.hpp"
#include "vkwp/device.hpp"
#include "vkwp/device_memory.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::Buffer::Buffer() = default;

vkwp::Buffer::Buffer(const std::string & label)
    : Resource{ label }
{}

vkwp::Buffer
vkwp::Buffer::create(const std::string &        label,
                     const Device &             device,
                     const VkDeviceSize         size,
                     const VkBufferUsageFlags   usage,
                     const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);

    Buffer                   ret{ label };
    const VkBufferCreateInfo buffer_create_info{
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = opt_args.p_next,
        .flags                 = opt_args.flags,
        .size                  = size,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
    };
    VulkanError::check(  //
        vkCreateBuffer(static_cast<VkDevice>(device),
                       &buffer_create_info,
                       nullptr,
                       ret.reset(static_cast<VkDevice>(device))),
        "Failed to vkCreateBuffer().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(static_cast<VkDevice>(device),
                                                         VK_OBJECT_TYPE_BUFFER,
                                                         static_cast<VkBuffer>(ret),
                                                         label);
    }
    return ret;
}

VkMemoryRequirements
vkwp::Buffer::getMemoryRequirements() const
{
    VKWP_VALIDATE_THIS;
    VkMemoryRequirements ret;
    vkGetBufferMemoryRequirements(this->_getParent(), this->_ref(), &ret);
    return ret;
}

void
vkwp::Buffer::bindMemory(const DeviceMemory & device_memory, const VkDeviceSize offset) const
{
    VKWP_VALIDATE_THIS;
    VKWP_VALIDATE_ARG(device_memory);
    VulkanError::check(  //
        vkBindBufferMemory(this->_getParent(), this->_ref(), static_cast<VkDeviceMemory>(device_memory), offset),
        "Failed to vkBindImageMemory().");
}
