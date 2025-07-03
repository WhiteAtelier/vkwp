#include "vkwp/descriptor_set.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/descriptor_pool.hpp"
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
vkwp::impl::_delete<VkDescriptorSet, VkDevice, VkDescriptorPool>(
    VkDescriptorSet                                i,
    const std::tuple<VkDevice, VkDescriptorPool> & args) noexcept
{
    // VkDescriptorPool を保持している時のみ解放する.
    if (std::get<1>(args) != nullptr)
    {
        vkFreeDescriptorSets(std::get<0>(args), std::get<1>(args), 1, &i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DescriptorSet class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
std::vector<vkwp::DescriptorSet>
vkwp::DescriptorSet::allocate(const DescriptorPool &                         descriptor_pool,
                              const std::vector<DescriptorSetAllocateInfo> & allocate_info_list,
                              const OptionalAllocateArgs &                   opt_args)
{
    VKWP_VALIDATE_ARG(descriptor_pool);
    std::vector<DescriptorSet>         ret;
    std::vector<VkDescriptorSetLayout> dset_layouts;
    ret.reserve(allocate_info_list.size());
    dset_layouts.reserve(allocate_info_list.size());
    for (const auto & ai : allocate_info_list)
    {
        VKWP_VALIDATE_ARG(ai.descriptor_set_layout != nullptr);
        dset_layouts.emplace_back(*ai.descriptor_set_layout);
        ret.emplace_back(DescriptorSet{ ai.label });
    }

    const auto device = std::get<0>(descriptor_pool.parents_);
    const auto require_to_free
        = (descriptor_pool.getCreateFlags() & VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT) != 0;

    std::vector<VkDescriptorSet> vk_descriptor_sets(dset_layouts.size(), nullptr);

    const VkDescriptorSetAllocateInfo descriptor_set_allocate_info{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = static_cast<std::uint32_t>(allocate_info_list.size()),
        .pSetLayouts        = dset_layouts.data(),
    };
    VulkanError::check(  //
        vkAllocateDescriptorSets(device, &descriptor_set_allocate_info, vk_descriptor_sets.data()),
        "Failed to vkAllocateDescriptorSets().");

    for (std::size_t i = 0; i < allocate_info_list.size(); ++i)
    {
        *ret[i]._reset(device, require_to_free ? descriptor_pool : nullptr) = vk_descriptor_sets[i];
    }
    if (opt_args.debug_utils != nullptr)
    {
        for (const auto & dset : ret)
        {
            opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                             VK_OBJECT_TYPE_DESCRIPTOR_SET,
                                                             static_cast<VkDescriptorSet>(dset),
                                                             dset.getLabel());
        }
    }

    return ret;
}