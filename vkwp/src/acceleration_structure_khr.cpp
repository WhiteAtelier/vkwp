#include "vkwp/acceleration_structure_khr.hpp"

#include "vkwp/buffer.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::AccelerationStructureKHR::AccelerationStructureKHR() = default;

vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::create(const std::string &                  label,
                                       const Device &                       device,
                                       const Buffer &                       buffer,
                                       const VkDeviceSize                   offset,
                                       const VkDeviceSize                   size,
                                       const VkAccelerationStructureTypeKHR type,
                                       const OptionalCreateArgs &           opt_args)
{
    AccelerationStructureKHR                   ret{ label };
    const VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info_khr{
        .sType         = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
        .pNext         = opt_args.p_next,
        .createFlags   = opt_args.create_flags,
        .buffer        = buffer,
        .offset        = offset,
        .size          = size,
        .type          = type,
        .deviceAddress = opt_args.device_address,
    };
    VulkanError::check(  //
        vkCreateAccelerationStructureKHR(device, &acceleration_structure_create_info_khr, nullptr, ret.reset(*device)),
        "Failed to vkCreateAccelerationStructureKHR().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, ret, label);
    }
    return ret;
}
