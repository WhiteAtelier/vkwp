#include "vkwp/pipeline.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/pipeline_cache.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkPipeline, VkDevice>(VkPipeline i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyPipeline(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// RayTracingPipelineKHR_ExtFunctions structr
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::RayTracingPipelineKHR_ExtFunctions
vkwp::RayTracingPipelineKHR_ExtFunctions::create(const VkInstance instance)
{
    RayTracingPipelineKHR_ExtFunctions ret;

#define GET_INSTANCE_PROC_ADDR(FuncName) \
    ret.FuncName = reinterpret_cast<PFN_##FuncName>(vkGetInstanceProcAddr(instance, #FuncName));

    GET_INSTANCE_PROC_ADDR(vkCreateRayTracingPipelinesKHR);

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Pipeline class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                  device,  //
                       const std::vector<GraphicsPipelineCreateInfo> & create_info_list)
{
    return Pipeline::create(device, create_info_list, {});
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                  device,
                       const std::vector<GraphicsPipelineCreateInfo> & create_info_list,
                       const OptionalCreateArgs &                      opt_args)
{
    VKWP_VALIDATE_ARG(device);
    std::vector<Pipeline>                     ret;
    std::vector<VkGraphicsPipelineCreateInfo> cis;
    ret.reserve(create_info_list.size());
    cis.reserve(create_info_list.size());
    for (const auto & ci : create_info_list)
    {
        cis.emplace_back(ci.create_info);
        ret.emplace_back(Pipeline{ ci.label });
    }

    std::vector<VkPipeline> vk_pipelines(create_info_list.size(), nullptr);

    VulkanError::check(  //
        vkCreateGraphicsPipelines(device,
                                  opt_args.pipeline_cache != nullptr  //
                                      ? static_cast<VkPipelineCache>(*opt_args.pipeline_cache)
                                      : nullptr,
                                  static_cast<std::uint32_t>(cis.size()),
                                  cis.data(),
                                  nullptr,
                                  vk_pipelines.data()),
        "Failed to vkCreateGraphicsPipelines().");

    for (std::size_t i = 0; i < create_info_list.size(); ++i)
    {
        *ret[i]._reset(device) = vk_pipelines[i];
    }
    if (opt_args.debug_utils != nullptr)
    {
        for (const auto & pipeline : ret)
        {
            opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                             VK_OBJECT_TYPE_PIPELINE,
                                                             static_cast<VkPipeline>(pipeline),
                                                             pipeline.getLabel());
        }
    }

    return ret;
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                 device,  //
                       const std::vector<ComputePipelineCreateInfo> & create_info_list)
{
    return Pipeline::create(device, create_info_list, {});
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                 device,
                       const std::vector<ComputePipelineCreateInfo> & create_info_list,
                       const OptionalCreateArgs &                     opt_args)
{
    VKWP_VALIDATE_ARG(device);
    std::vector<Pipeline>                    ret;
    std::vector<VkComputePipelineCreateInfo> cis;
    ret.reserve(create_info_list.size());
    cis.reserve(create_info_list.size());
    for (const auto & ci : create_info_list)
    {
        cis.emplace_back(ci.create_info);
        ret.emplace_back(Pipeline{ ci.label });
    }

    std::vector<VkPipeline> vk_pipelines(create_info_list.size(), nullptr);

    VulkanError::check(  //
        vkCreateComputePipelines(device,
                                 opt_args.pipeline_cache != nullptr  //
                                     ? static_cast<VkPipelineCache>(*opt_args.pipeline_cache)
                                     : nullptr,
                                 static_cast<std::uint32_t>(cis.size()),
                                 cis.data(),
                                 nullptr,
                                 vk_pipelines.data()),
        "Failed to vkCreateComputePipelines().");

    for (std::size_t i = 0; i < create_info_list.size(); ++i)
    {
        *ret[i]._reset(device) = vk_pipelines[i];
    }
    if (opt_args.debug_utils != nullptr)
    {
        for (const auto & pipeline : ret)
        {
            opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                             VK_OBJECT_TYPE_PIPELINE,
                                                             static_cast<VkPipeline>(pipeline),
                                                             pipeline.getLabel());
        }
    }

    return ret;
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                       device,
                       const std::vector<RayTracingPipelineCreateInfoKHR> & create_info_list,
                       const RayTracingPipelineKHR_ExtFunctions &           ext_functions)
{
    return Pipeline::create(device, create_info_list, ext_functions, {});
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                       device,
                       const std::vector<RayTracingPipelineCreateInfoKHR> & create_info_list,
                       const RayTracingPipelineKHR_ExtFunctions &           ext_functions,
                       const OptionalCreateRTPipelineArgs &                 opt_args)
{
    VKWP_VALIDATE_ARG(device);
    std::vector<Pipeline>                          ret;
    std::vector<VkRayTracingPipelineCreateInfoKHR> cis;
    ret.reserve(create_info_list.size());
    cis.reserve(create_info_list.size());
    for (const auto & ci : create_info_list)
    {
        cis.emplace_back(ci.create_info);
        ret.emplace_back(Pipeline{ ci.label });
    }

    std::vector<VkPipeline> vk_pipelines(create_info_list.size(), nullptr);

    VulkanError::check(  //
        ext_functions.vkCreateRayTracingPipelinesKHR(device,
                                                     nullptr,  // *opt_args.deferred_operation,
                                                     opt_args.pipeline_cache != nullptr  //
                                                         ? static_cast<VkPipelineCache>(*opt_args.pipeline_cache)
                                                         : nullptr,
                                                     static_cast<std::uint32_t>(cis.size()),
                                                     cis.data(),
                                                     nullptr,
                                                     vk_pipelines.data()),
        "Failed to vkCreateRayTracingPipelinesKHR().");

    for (std::size_t i = 0; i < create_info_list.size(); ++i)
    {
        *ret[i]._reset(device) = vk_pipelines[i];
    }
    if (opt_args.debug_utils != nullptr)
    {
        for (const auto & pipeline : ret)
        {
            opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                             VK_OBJECT_TYPE_PIPELINE,
                                                             static_cast<VkPipeline>(pipeline),
                                                             pipeline.getLabel());
        }
    }

    return ret;
}
