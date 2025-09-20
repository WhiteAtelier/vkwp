#include "vkwp/semaphore.hpp"

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
vkwp::impl::_delete<VkSemaphore, VkDevice>(VkSemaphore i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroySemaphore(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineCache class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Semaphore
vkwp::Semaphore::create(const std::string & label, const Device & device, const OptionalCreateArgs & opt_args)
{
    Semaphore                   ret{ label };
    const VkSemaphoreCreateInfo ci{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = opt_args.p_next,
        .flags = opt_args.flags,
    };
    VulkanError::check(  //
        vkCreateSemaphore(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreateSemaphore().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_SEMAPHORE,
                                                         static_cast<VkSemaphore>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

const vkwp::Semaphore::OptionalCreateArgs vkwp::Semaphore::default_opt_args;