#include "vkwp/fence.hpp"

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
vkwp::impl::_delete(VkFence i, PFN_vkDestroyFence fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Image class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Fence
vkwp::Fence::create(std::string label, const Device & device)
{
    return Fence::create(std::move(label), device, {});
}

/* STATIC */
vkwp::Fence
vkwp::Fence::create(std::string label, const Device & device, const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    Fence                   ret{ std::move(label) };
    const VkFenceCreateInfo ci{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = opt_args.p_next,
        .flags = opt_args.flags,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateFence != nullptr)
                                ? opt_args.functions->vkCreateFence
                                : vkCreateFence;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyFence != nullptr)
                                ? opt_args.functions->vkDestroyFence
                                : vkDestroyFence;

    VulkanError::check(create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),  //
                       "Failed to vkCreateFence().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_FENCE,
                                                         static_cast<VkFence>(ret),
                                                         ret.getLabel());
    }
    if (opt_args.functions != nullptr)
    {
        ret._vkWaitForFences
            = (opt_args.functions->vkWaitForFences != nullptr) ? opt_args.functions->vkWaitForFences : vkWaitForFences;
        ret._vkResetFences
            = (opt_args.functions->vkResetFences != nullptr) ? opt_args.functions->vkResetFences : vkResetFences;
    }
    return ret;
}

bool
vkwp::Fence::wait(const std::uint64_t timeout) const
{
    VKWP_VALIDATE_THIS;
    const auto result = this->_vkWaitForFences(std::get<0>(this->parents_), 1, this->ptr(), VK_TRUE, timeout);
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
        this->_vkResetFences(std::get<0>(this->parents_), 1, this->ptr()),
        "Failed to vkResetFences().");
}
