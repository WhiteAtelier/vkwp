#include "vkwp/acceleration_structure_khr.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/buffer.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkAccelerationStructureKHR>(
    VkAccelerationStructureKHR                                          i,
    const std::tuple<VkDevice, AccelerationStructureKHR_ExtFunctions> & args) noexcept
{
    VKWP_ASSERTION(std::get<1>(args).vkDestroyAccelerationStructureKHR != nullptr);
    if (const auto & ext_functions = std::get<1>(args); ext_functions.vkDestroyAccelerationStructureKHR != nullptr)
    {
        ext_functions.vkDestroyAccelerationStructureKHR(std::get<0>(args), i, nullptr);
    }
#if 0
    else
    {
        vkDestroyAccelerationStructureKHR(std::get<0>(args), i, nullptr);
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// AccelerationStructureKHR_ExtFunctions class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::AccelerationStructureKHR_ExtFunctions
vkwp::AccelerationStructureKHR_ExtFunctions::create(const VkInstance instance)
{
    AccelerationStructureKHR_ExtFunctions ret;

#define GET_INSTANCE_PROC_ADDR(FuncName) \
    ret.FuncName = reinterpret_cast<PFN_##FuncName>(vkGetInstanceProcAddr(instance, #FuncName));

    GET_INSTANCE_PROC_ADDR(vkCreateAccelerationStructureKHR);
    GET_INSTANCE_PROC_ADDR(vkDestroyAccelerationStructureKHR);

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// AccelerationStructureKHR class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::create(const std::string_view               label,
                                       const Device &                       device,
                                       const Buffer &                       buffer,
                                       const VkDeviceSize                   offset,
                                       const VkDeviceSize                   size,
                                       const VkAccelerationStructureTypeKHR type)
{
    return AccelerationStructureKHR::create(label,  //
                                            device,
                                            buffer,
                                            offset,
                                            size,
                                            type,
                                            {});
}

vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::create(const std::string_view               label,
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
        vkCreateAccelerationStructureKHR(device,
                                         &acceleration_structure_create_info_khr,
                                         nullptr,
                                         ret._reset(device, {})),
        "Failed to vkCreateAccelerationStructureKHR().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, ret, label);
    }
    return ret;
}
#endif

/* STATIC */
vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::createEXT(const std::string_view                        label,
                                          const Device &                                device,
                                          const Buffer &                                buffer,
                                          const VkDeviceSize                            offset,
                                          const VkDeviceSize                            size,
                                          const VkAccelerationStructureTypeKHR          type,
                                          const AccelerationStructureKHR_ExtFunctions & ext_functions)
{
    return AccelerationStructureKHR::createEXT(label,  //
                                               device,
                                               buffer,
                                               offset,
                                               size,
                                               type,
                                               ext_functions,
                                               {});
}

/* STATIC */
vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::createEXT(const std::string_view                        label,
                                          const Device &                                device,
                                          const Buffer &                                buffer,
                                          const VkDeviceSize                            offset,
                                          const VkDeviceSize                            size,
                                          const VkAccelerationStructureTypeKHR          type,
                                          const AccelerationStructureKHR_ExtFunctions & ext_functions,
                                          const OptionalCreateArgs &                    opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(buffer);
    VKWP_VALIDATE_ARG(ext_functions.vkCreateAccelerationStructureKHR);
    VKWP_VALIDATE_ARG(ext_functions.vkDestroyAccelerationStructureKHR);
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
        ext_functions.vkCreateAccelerationStructureKHR(device,
                                                       &acceleration_structure_create_info_khr,
                                                       nullptr,
                                                       ret._reset(device, ext_functions)),
        "Failed to vkCreateAccelerationStructureKHR().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, ret, label);
    }
    return ret;
}