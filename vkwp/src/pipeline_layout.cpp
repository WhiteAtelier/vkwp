#include "vkwp/pipeline_layout.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/descriptor_set_layout.hpp"
#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkPipelineLayout, VkDevice>(VkPipelineLayout i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyPipelineLayout(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineLayout class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::PipelineLayout
vkwp::PipelineLayout::createPipelineLayout(
    const std::string &                                              label,
    const Device &                                                   device,
    const std::vector<std::reference_wrapper<DescriptorSetLayout>> & _set_layouts,
    const std::vector<VkPushConstantRange> &                         push_constant_ranges,
    const OptionalCreateArgs &                                       opt_args)
{
    VKWP_VALIDATE_ARG(device);

    std::vector<VkDescriptorSetLayout> set_layouts;
    set_layouts.reserve(_set_layouts.size());

    for (const auto & layout : _set_layouts)
    {
        VKWP_VALIDATE_ARG(layout.get());
        set_layouts.emplace_back(layout.get());
    }

    PipelineLayout                   ret{ label };
    const VkPipelineLayoutCreateInfo ci{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = opt_args.p_next,
        .flags                  = opt_args.flags,
        .setLayoutCount         = static_cast<std::uint32_t>(set_layouts.size()),
        .pSetLayouts            = set_layouts.data(),
        .pushConstantRangeCount = static_cast<std::uint32_t>(push_constant_ranges.size()),
        .pPushConstantRanges    = push_constant_ranges.data(),
    };
    VulkanError::check(  //
        vkCreatePipelineLayout(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreatePipelineLayout().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                                                         static_cast<VkPipelineLayout>(ret),
                                                         ret.getLabel());
    }
    return ret;
}