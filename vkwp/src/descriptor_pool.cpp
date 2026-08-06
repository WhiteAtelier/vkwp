#include "vkwp/descriptor_pool.hpp"

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
vkwp::impl::_delete(VkDescriptorPool i, PFN_vkDestroyDescriptorPool fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandPool class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::DescriptorPool
vkwp::DescriptorPool::create(const std::string_view                    label,
                             const Device &                            device,
                             const std::uint32_t                       max_sets,
                             const std::vector<VkDescriptorPoolSize> & pool_sizes)
{
    return DescriptorPool::create(label, device, max_sets, pool_sizes, {});
}

/* STATIC */
vkwp::DescriptorPool
vkwp::DescriptorPool::create(const std::string_view                    label,
                             const Device &                            device,
                             const std::uint32_t                       max_sets,
                             const std::vector<VkDescriptorPoolSize> & pool_sizes,
                             const OptionalCreateArgs &                opt_args)
{
    VKWP_VALIDATE_ARG(device);
    DescriptorPool                   ret{ label };
    const VkDescriptorPoolCreateInfo ci{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = opt_args.p_next,
        .flags         = opt_args.flags,
        .maxSets       = max_sets,
        .poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data(),
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateDescriptorPool != nullptr)
                                ? opt_args.functions->vkCreateDescriptorPool
                                : vkCreateDescriptorPool;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyDescriptorPool != nullptr)
                                ? opt_args.functions->vkDestroyDescriptorPool
                                : vkDestroyDescriptorPool;

    VulkanError::check(create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),  //
                       "Failed to vkCreateDescriptorPool().");
    ret.create_flags_ = opt_args.flags;
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_DESCRIPTOR_POOL,
                                                         static_cast<VkDescriptorPool>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

VkDescriptorPoolCreateFlags
vkwp::DescriptorPool::getCreateFlags() const noexcept
{
    return this->create_flags_;
}
