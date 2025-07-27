
#include "vkwp/buffer.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/device_memory.hpp"
#include "vkwp/device_memory_node.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkBuffer>(VkBuffer i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyBuffer(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Buffer class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
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
        vkCreateBuffer(device, &buffer_create_info, nullptr, ret._reset(device)),
        "Failed to vkCreateBuffer().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device, VK_OBJECT_TYPE_BUFFER, ret, label);
    }
    return ret;
}

VkMemoryRequirements
vkwp::Buffer::getMemoryRequirements() const
{
    VKWP_VALIDATE_THIS;
    VkMemoryRequirements ret;
    vkGetBufferMemoryRequirements(std::get<0>(this->parents_), *this, &ret);
    return ret;
}

void
vkwp::Buffer::bindMemory(const DeviceMemory & device_memory, const VkDeviceSize offset) const
{
    VKWP_VALIDATE_THIS;
    VKWP_VALIDATE_ARG(device_memory);
    VulkanError::check(  //
        vkBindBufferMemory(std::get<0>(this->parents_), *this, device_memory, offset),
        "Failed to vkBindImageMemory().");
}

void
vkwp::Buffer::bindMemory(const DeviceMemoryNode & device_memory_node, const VkDeviceSize offset) const
{
    VKWP_VALIDATE_THIS;
    VKWP_VALIDATE_ARG(device_memory_node);
    VulkanError::check(  //
        vkBindBufferMemory(std::get<0>(this->parents_),
                           *this,
                           device_memory_node._getDeviceMemory(),
                           device_memory_node._getOffset() + offset),
        "Failed to vkBindImageMemory().");
}

VkDeviceAddress
vkwp::Buffer::getBufferDeviceAddress() const
{
    VKWP_VALIDATE_THIS;
    const VkBufferDeviceAddressInfo buffer_device_address_info{
        .sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .pNext  = nullptr,
        .buffer = *this,
    };
    return vkGetBufferDeviceAddress(std::get<0>(this->parents_), &buffer_device_address_info);
}
