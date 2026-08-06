
#include "vkwp/buffer.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/device_memory.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkBuffer i, PFN_vkDestroyBuffer fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Buffer class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::Buffer
vkwp::Buffer::create(const std::string_view   label,
                     const Device &           device,
                     const VkDeviceSize       size,
                     const VkBufferUsageFlags usage)
{
    return create(label, device, size, usage, {});
}

vkwp::Buffer
vkwp::Buffer::create(const std::string_view     label,
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
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateBuffer != nullptr)
                                ? opt_args.functions->vkCreateBuffer
                                : vkCreateBuffer;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyBuffer != nullptr)
                                ? opt_args.functions->vkDestroyBuffer
                                : vkDestroyBuffer;

    VulkanError::check(create_fn(device, &buffer_create_info, nullptr, ret._reset(destroy_fn, device)),  //
                       "Failed to vkCreateBuffer().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device, VK_OBJECT_TYPE_BUFFER, ret, label);
    }

    if (opt_args.functions != nullptr)
    {
        ret._vkGetBufferMemoryRequirements = (opt_args.functions->vkGetBufferMemoryRequirements != nullptr)
                                               ? opt_args.functions->vkGetBufferMemoryRequirements
                                               : vkGetBufferMemoryRequirements;
        ret._vkBindBufferMemory            = (opt_args.functions->vkBindBufferMemory != nullptr)
                                               ? opt_args.functions->vkBindBufferMemory
                                               : vkBindBufferMemory;
        ret._vkGetBufferDeviceAddress      = (opt_args.functions->vkGetBufferDeviceAddress != nullptr)
                                               ? opt_args.functions->vkGetBufferDeviceAddress
                                               : vkGetBufferDeviceAddress;
    }

    return ret;
}

VkMemoryRequirements
vkwp::Buffer::getMemoryRequirements() const
{
    VKWP_VALIDATE_THIS;
    VkMemoryRequirements ret;
    this->_vkGetBufferMemoryRequirements(std::get<0>(this->parents_), *this, &ret);
    return ret;
}

void
vkwp::Buffer::bindMemory(const DeviceMemory & device_memory, const VkDeviceSize offset) const
{
    VKWP_VALIDATE_THIS;
    VKWP_VALIDATE_ARG(device_memory);
    VulkanError::check(this->_vkBindBufferMemory(std::get<0>(this->parents_), *this, device_memory, offset),
                       "Failed to vkBindBufferMemory().");
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
    return this->_vkGetBufferDeviceAddress(std::get<0>(this->parents_), &buffer_device_address_info);
}
