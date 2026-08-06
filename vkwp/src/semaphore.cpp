#include "vkwp/semaphore.hpp"

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
vkwp::impl::_delete(VkSemaphore i, PFN_vkDestroySemaphore fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineCache class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Semaphore
vkwp::Semaphore::create(std::string label, const Device & device)
{
    return Semaphore::create(std::move(label), device, {});
}

/* STATIC */
vkwp::Semaphore
vkwp::Semaphore::create(std::string label, const Device & device, const OptionalCreateArgs & opt_args)
{
    Semaphore                   ret{ std::move(label) };
    const VkSemaphoreCreateInfo ci{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = opt_args.p_next,
        .flags = opt_args.flags,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateSemaphore != nullptr)
                                ? opt_args.functions->vkCreateSemaphore
                                : vkCreateSemaphore;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroySemaphore != nullptr)
                                ? opt_args.functions->vkDestroySemaphore
                                : vkDestroySemaphore;

    VulkanError::check(  //
        create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),
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
