#include "vkwp/descriptor_set_layout.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

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
vkwp::impl::_delete(VkDescriptorSetLayout            i,
                    PFN_vkDestroyDescriptorSetLayout fn,
                    const std::tuple<VkDevice> &     args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
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
    auto * const create_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkCreateDescriptorSetLayout != nullptr)
            ? opt_args.functions->vkCreateDescriptorSetLayout
            : vkCreateDescriptorSetLayout;
    auto * const destroy_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkDestroyDescriptorSetLayout != nullptr)
            ? opt_args.functions->vkDestroyDescriptorSetLayout
            : vkDestroyDescriptorSetLayout;

    VulkanError::check(create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),  //
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
