#include "vkwp/acceleration_structure_khr.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/buffer.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

namespace {

vkwp::AccelerationStructureKHR_ExtFunctions
resolveFunctions(const VkDevice                                      device,
                 const vkwp::AccelerationStructureKHR_ExtFunctions & ext_functions,
                 const vkwp::VulkanFunctions * const                 functions)
{
    auto ret = ext_functions;

    if (functions != nullptr)
    {
        if (functions->vkCreateAccelerationStructureKHR != nullptr)
        {
            ret.vkCreateAccelerationStructureKHR = functions->vkCreateAccelerationStructureKHR;
        }
        if (functions->vkDestroyAccelerationStructureKHR != nullptr)
        {
            ret.vkDestroyAccelerationStructureKHR = functions->vkDestroyAccelerationStructureKHR;
        }
    }

    auto * const get_device_proc_addr_fn = (functions != nullptr && functions->vkGetDeviceProcAddr != nullptr)
                                             ? functions->vkGetDeviceProcAddr
                                             : vkGetDeviceProcAddr;
    if (ret.vkCreateAccelerationStructureKHR == nullptr)
    {
        ret.vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(
            get_device_proc_addr_fn(device, "vkCreateAccelerationStructureKHR"));
    }
    if (ret.vkDestroyAccelerationStructureKHR == nullptr)
    {
        ret.vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(
            get_device_proc_addr_fn(device, "vkDestroyAccelerationStructureKHR"));
    }

    return ret;
}

}  // namespace

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
// AccelerationStructureKHR_ExtFunctions class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::AccelerationStructureKHR_ExtFunctions
vkwp::AccelerationStructureKHR_ExtFunctions::create(const VkDevice device, const VulkanFunctions * functions)
{
    return resolveFunctions(device, {}, functions);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// AccelerationStructureKHR class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::AccelerationStructureKHR
vkwp::AccelerationStructureKHR::create(const std::string_view               label,
                                       const Device &                       device,
                                       const Buffer &                       buffer,
                                       const VkDeviceSize                   offset,
                                       const VkDeviceSize                   size,
                                       const VkAccelerationStructureTypeKHR type)
{
    return AccelerationStructureKHR::create(label, device, buffer, offset, size, type, {});
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
    return AccelerationStructureKHR::createEXT(label,
                                               device,
                                               buffer,
                                               offset,
                                               size,
                                               type,
                                               AccelerationStructureKHR_ExtFunctions{},
                                               opt_args);
}

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
    const auto resolved_functions = resolveFunctions(device, ext_functions, opt_args.functions);
    VKWP_VALIDATE_ARG(resolved_functions.vkCreateAccelerationStructureKHR);
    VKWP_VALIDATE_ARG(resolved_functions.vkDestroyAccelerationStructureKHR);
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
        resolved_functions.vkCreateAccelerationStructureKHR(
            device,
            &acceleration_structure_create_info_khr,
            nullptr,
            ret._reset(resolved_functions.vkDestroyAccelerationStructureKHR, device)),
        "Failed to vkCreateAccelerationStructureKHR().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, ret, label);
    }
    return ret;
}
