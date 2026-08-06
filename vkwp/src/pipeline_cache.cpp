#include "vkwp/pipeline_cache.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/descriptor_set_layout.hpp"
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
vkwp::impl::_delete(VkPipelineCache i, PFN_vkDestroyPipelineCache fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineCache class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::PipelineCache
vkwp::PipelineCache::create(std::string label, const Device & device)
{
    return PipelineCache::create(std::move(label), device, {});
}

/* STATIC */
vkwp::PipelineCache
vkwp::PipelineCache::create(std::string label, const Device & device, const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    vkwp::PipelineCache             ret{ std::move(label) };
    const VkPipelineCacheCreateInfo pipeline_cache_create_info{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext           = opt_args.p_next,
        .flags           = opt_args.flags,
        .initialDataSize = opt_args.initial_data_size,
        .pInitialData    = opt_args.initial_data,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreatePipelineCache != nullptr)
                                ? opt_args.functions->vkCreatePipelineCache
                                : vkCreatePipelineCache;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyPipelineCache != nullptr)
                                ? opt_args.functions->vkDestroyPipelineCache
                                : vkDestroyPipelineCache;

    VulkanError::check(create_fn(device, &pipeline_cache_create_info, nullptr, ret._reset(destroy_fn, device)),
                       "Failed to vkCreatePipelineCache().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_PIPELINE_CACHE,
                                                         static_cast<VkPipelineCache>(ret),
                                                         ret.getLabel());
    }
    return ret;
}
