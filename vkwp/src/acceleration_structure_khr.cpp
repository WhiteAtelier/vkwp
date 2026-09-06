#include "vkwp/acceleration_structure_khr.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/buffer.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkAccelerationStructureKHR            i,
                    PFN_vkDestroyAccelerationStructureKHR fn,
                    const std::tuple<VkDevice> &          args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// AccelerationStructureKHR class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::create(std::string                          label,
                                       const Device &                       device,
                                       const Buffer &                       buffer,
                                       const VkDeviceSize                   offset,
                                       const VkDeviceSize                   size,
                                       const VkAccelerationStructureTypeKHR type)
{
    return AccelerationStructureKHR::create(std::move(label), device, buffer, offset, size, type, {});
}

vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::create(std::string                          label,
                                       const Device &                       device,
                                       const Buffer &                       buffer,
                                       const VkDeviceSize                   offset,
                                       const VkDeviceSize                   size,
                                       const VkAccelerationStructureTypeKHR type,
                                       const OptionalCreateArgs &           opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(buffer);

    PFN_vkCreateAccelerationStructureKHR  create_fn               = nullptr;
    PFN_vkDestroyAccelerationStructureKHR destroy_fn              = nullptr;
    PFN_vkGetDeviceProcAddr               get_device_proc_addr_fn = vkGetDeviceProcAddr;
    if (opt_args.functions != nullptr)
    {
        create_fn  = opt_args.functions->vkCreateAccelerationStructureKHR;
        destroy_fn = opt_args.functions->vkDestroyAccelerationStructureKHR;
        if (opt_args.functions->vkGetDeviceProcAddr != nullptr)
        {
            get_device_proc_addr_fn = opt_args.functions->vkGetDeviceProcAddr;
        }
    }
    if (create_fn == nullptr)
    {
        create_fn = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(
            get_device_proc_addr_fn(device, "vkCreateAccelerationStructureKHR"));
    }
    if (destroy_fn == nullptr)
    {
        destroy_fn = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(
            get_device_proc_addr_fn(device, "vkDestroyAccelerationStructureKHR"));
    }
    VKWP_VALIDATE_ARG(create_fn);
    VKWP_VALIDATE_ARG(destroy_fn);

    AccelerationStructureKHR                   ret{ std::move(label) };
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
        create_fn(device, &acceleration_structure_create_info_khr, nullptr, ret._reset(destroy_fn, device)),
        "Failed to vkCreateAccelerationStructureKHR().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR,
                                                         ret,
                                                         ret.getLabel());
    }
    return ret;
}

vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::importExternal(std::string                      label,
                                               const Device &                   device,
                                               const VkAccelerationStructureKHR external)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(external);
    return AccelerationStructureKHR{ std::move(label), external, static_cast<VkDevice>(device) };
}
