#include "vkwp/shader_module.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
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
vkwp::impl::_delete<VkShaderModule, VkDevice>(VkShaderModule i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyShaderModule(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineCache class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::ShaderModule
vkwp::ShaderModule::createShaderModule(const std::string &                label,
                                       const Device &                     device,
                                       const std::vector<std::uint32_t> & code,
                                       const OptionalCreateArgs &         opt_args)
{
    return createShaderModule(label, device, code.data(), code.size(), opt_args);
}

vkwp::ShaderModule
vkwp::ShaderModule::createShaderModule(const std::string &         label,
                                       const Device &              device,
                                       const std::uint32_t * const data,
                                       const std::size_t           data_length,
                                       const OptionalCreateArgs &  opt_args)
{
    VKWP_VALIDATE_ARG(device);
    ShaderModule                   ret{ label };
    const VkShaderModuleCreateInfo ci{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = opt_args.p_next,
        .flags    = opt_args.flags,
        .codeSize = data_length * sizeof(std::uint32_t),
        .pCode    = data,
    };
    VulkanError::check(  //
        vkCreateShaderModule(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreateShaderModule().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_SHADER_MODULE,
                                                         static_cast<VkShaderModule>(ret),
                                                         ret.getLabel());
    }
    return ret;
}
