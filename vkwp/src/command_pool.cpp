#include "vkwp/command_pool.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/command_buffer.hpp"
#include "vkwp/device.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

#include <optional>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkCommandPool i, PFN_vkDestroyCommandPool fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandPool class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::CommandPool
vkwp::CommandPool::create(std::string         label,  //
                          const Device &      device,
                          const std::uint32_t queue_family_index)
{
    return create(std::move(label), device, queue_family_index, nullptr, {});
}

/* STATIC */
vkwp::CommandPool
vkwp::CommandPool::create(std::string               label,
                          const Device &            device,
                          const std::uint32_t       queue_family_index,
                          std::unique_ptr<Mutex> && mutex)
{
    return create(std::move(label), device, queue_family_index, std::move(mutex), {});
}

/* STATIC */
vkwp::CommandPool
vkwp::CommandPool::create(std::string                label,
                          const Device &             device,
                          const std::uint32_t        queue_family_index,
                          std::unique_ptr<Mutex> &&  mutex,
                          const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    CommandPool                   ret{ std::move(label) };
    const VkCommandPoolCreateInfo ci{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = opt_args.p_next,
        .flags            = opt_args.flags,
        .queueFamilyIndex = queue_family_index,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateCommandPool != nullptr)
                                ? opt_args.functions->vkCreateCommandPool
                                : vkCreateCommandPool;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyCommandPool != nullptr)
                                ? opt_args.functions->vkDestroyCommandPool
                                : vkDestroyCommandPool;

    VulkanError::check(create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),  //
                       "Failed to vkCreateCommandPool().");

    ret.create_flags_ = opt_args.flags;
    ret.mutex_        = std::move(mutex);

    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_COMMAND_POOL,
                                                         static_cast<VkCommandPool>(ret),
                                                         ret.getLabel());
    }
    if (opt_args.functions != nullptr)
    {
        ret._vkResetCommandPool = (opt_args.functions->vkResetCommandPool != nullptr)
                                    ? opt_args.functions->vkResetCommandPool
                                    : vkResetCommandPool;
    }
    return ret;
}

VkCommandPoolCreateFlags
vkwp::CommandPool::getCreateFlags() const noexcept
{
    return this->create_flags_;
}

void
vkwp::CommandPool::resetCommandPool(const VkCommandPoolResetFlags flags) const
{
    VulkanError::check(this->_vkResetCommandPool(std::get<0>(this->parents_), *this, flags),
                       "Failed to vkResetCommandPool()");
}
