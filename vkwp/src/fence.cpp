#include "vkwp/fence.hpp"

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
vkwp::impl::_delete<VkFence, VkDevice>(VkFence i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyFence(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Image class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Fence
vkwp::Fence::create(const std::string & label, const Device & device, const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    Fence                   ret{ label };
    const VkFenceCreateInfo ci{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = opt_args.p_next,
        .flags = opt_args.flags,
    };
    VulkanError::check(  //
        vkCreateFence(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreateFence().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_FENCE,
                                                         static_cast<VkFence>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

bool
vkwp::Fence::wait(const std::uint64_t timeout) const
{
    VKWP_VALIDATE_THIS;
    const auto result = vkWaitForFences(std::get<0>(this->parents_), 1, this->ptr(), VK_TRUE, timeout);
    if (result == VK_TIMEOUT)
    {
        return false;
    }
    VulkanError::check(result, "Failed to vkWaitForFences().");
    return true;
}

void
vkwp::Fence::resetSignal() const
{
    VKWP_VALIDATE_THIS;
    VulkanError::check(  //
        vkResetFences(std::get<0>(this->parents_), 1, this->ptr()),
        "Failed to vkResetFences().");
}

const vkwp::Fence::OptionalCreateArgs vkwp::Fence::default_opt_args;