#include "vkwp/pipeline_cache.hpp"

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
vkwp::impl::_delete<VkPipelineCache, VkDevice>(VkPipelineCache i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyPipelineCache(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineCache class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::PipelineCache
vkwp::PipelineCache::create(const std::string & label, const Device & device, const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    vkwp::PipelineCache             ret{ label };
    const VkPipelineCacheCreateInfo pipeline_cache_create_info{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext           = opt_args.p_next,
        .flags           = opt_args.flags,
        .initialDataSize = opt_args.initial_data_size,
        .pInitialData    = opt_args.initial_data,
    };
    vkCreatePipelineCache(device, &pipeline_cache_create_info, nullptr, ret._reset(device));
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_PIPELINE_CACHE,
                                                         static_cast<VkPipelineCache>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

const vkwp::PipelineCache::OptionalCreateArgs vkwp::PipelineCache::default_opt_args;