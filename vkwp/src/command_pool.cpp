#include "vkwp/command_pool.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/command_buffer.hpp"
#include "vkwp/device.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/vulkan_error.hpp"

#include <optional>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkCommandPool, VkDevice>(VkCommandPool i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyCommandPool(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandPool class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::CommandPool
vkwp::CommandPool::create(const std::string &        label,
                          const Device &             device,
                          const std::uint32_t        queue_family_index,
                          const OptionalCreateArgs & opt_args)
{
    return create(label, device, queue_family_index, nullptr, opt_args);
}

/* STATIC */
vkwp::CommandPool
vkwp::CommandPool::create(const std::string &        label,
                          const Device &             device,
                          const std::uint32_t        queue_family_index,
                          std::unique_ptr<Mutex> &&  mutex,
                          const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    CommandPool                   ret{ label };
    const VkCommandPoolCreateInfo ci{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = opt_args.p_next,
        .flags            = opt_args.flags,
        .queueFamilyIndex = queue_family_index,
    };
    VulkanError::check(  //
        vkCreateCommandPool(device, &ci, nullptr, ret._reset(device)),
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
    return ret;
}

VkCommandPoolCreateFlags
vkwp::CommandPool::getCreateFlags() const noexcept
{
    return this->create_flags_;
}
