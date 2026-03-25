#include "vkwp/descriptor_set_layout.hpp"

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
vkwp::impl::_delete<VkDescriptorSetLayout, VkDevice>(VkDescriptorSetLayout        i,
                                                     const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyDescriptorSetLayout(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DescriptorSetLayout class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::DescriptorSetLayout
vkwp::DescriptorSetLayout::create(const std::string_view                            label,
                                  const Device &                                    device,
                                  const std::vector<VkDescriptorSetLayoutBinding> & bindings)
{
    return DescriptorSetLayout::create(label,  //
                                       device,
                                       bindings,
                                       {});
}

/* STATIC */
vkwp::DescriptorSetLayout
vkwp::DescriptorSetLayout::create(const std::string_view                            label,
                                  const Device &                                    device,
                                  const std::vector<VkDescriptorSetLayoutBinding> & bindings,
                                  const OptionalCreateArgs &                        opt_args)
{
    VKWP_VALIDATE_ARG(device);
    DescriptorSetLayout                   ret{ label };
    const VkDescriptorSetLayoutCreateInfo ci{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = opt_args.p_next,
        .flags        = opt_args.flags,
        .bindingCount = static_cast<std::uint32_t>(bindings.size()),
        .pBindings    = bindings.data(),
    };
    VulkanError::check(  //
        vkCreateDescriptorSetLayout(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreateDescriptorSetLayout().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
                                                         static_cast<VkDescriptorSetLayout>(ret),
                                                         ret.getLabel());
    }
    return ret;
}