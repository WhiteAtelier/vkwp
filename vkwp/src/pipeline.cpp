// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "vkwp/pipeline.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/pipeline_cache.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkPipeline i, PFN_vkDestroyPipeline fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// RayTracingPipelineKHR_ExtFunctions structr
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::RayTracingPipelineKHR_ExtFunctions
vkwp::RayTracingPipelineKHR_ExtFunctions::create(const VkInstance instance, const VulkanFunctions * functions)
{
    RayTracingPipelineKHR_ExtFunctions ret;
    auto * const get_instance_proc_addr_fn = (functions != nullptr && functions->vkGetInstanceProcAddr != nullptr)
                                               ? functions->vkGetInstanceProcAddr
                                               : vkGetInstanceProcAddr;

#define GET_INSTANCE_PROC_ADDR(FuncName) \
    ret.FuncName = reinterpret_cast<PFN_##FuncName>(get_instance_proc_addr_fn(instance, #FuncName));

    GET_INSTANCE_PROC_ADDR(vkCreateRayTracingPipelinesKHR);

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Pipeline class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::Pipeline
vkwp::Pipeline::importExternal(std::string label, const Device & device, const VkPipeline external)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(external);
    return Pipeline{ std::move(label), external, static_cast<VkDevice>(device) };
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                          device,  //
                       std::vector<GraphicsPipelineCreateInfo> create_info_list)
{
    return Pipeline::create(device, std::move(create_info_list), {});
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                          device,
                       std::vector<GraphicsPipelineCreateInfo> create_info_list,
                       const OptionalCreateArgs &              opt_args)
{
    VKWP_VALIDATE_ARG(device);
    std::vector<Pipeline>                     ret;
    std::vector<VkGraphicsPipelineCreateInfo> cis;
    ret.reserve(create_info_list.size());
    cis.reserve(create_info_list.size());
    for (auto & ci : create_info_list)
    {
        cis.emplace_back(ci.create_info);
        ret.emplace_back(Pipeline{ std::move(ci.label) });
    }

    std::vector<VkPipeline> vk_pipelines(create_info_list.size(), nullptr);
    auto * const create_fn = (opt_args.functions != nullptr && opt_args.functions->vkCreateGraphicsPipelines != nullptr)
                               ? opt_args.functions->vkCreateGraphicsPipelines
                               : vkCreateGraphicsPipelines;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyPipeline != nullptr)
                                ? opt_args.functions->vkDestroyPipeline
                                : vkDestroyPipeline;

    const VkResult result = create_fn(device,
                                      opt_args.pipeline_cache != nullptr  //
                                          ? static_cast<VkPipelineCache>(*opt_args.pipeline_cache)
                                          : nullptr,
                                      static_cast<std::uint32_t>(cis.size()),
                                      cis.data(),
                                      nullptr,
                                      vk_pipelines.data());
    for (std::size_t i = 0; i < create_info_list.size(); ++i)
    {
        *ret[i]._reset(destroy_fn, device) = vk_pipelines[i];
    }
    VulkanError::check(result, "Failed to vkCreateGraphicsPipelines().");
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
vkwp::Pipeline::create(const Device &                         device,  //
                       std::vector<ComputePipelineCreateInfo> create_info_list)
{
    return Pipeline::create(device, std::move(create_info_list), {});
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                         device,
                       std::vector<ComputePipelineCreateInfo> create_info_list,
                       const OptionalCreateArgs &             opt_args)
{
    VKWP_VALIDATE_ARG(device);
    std::vector<Pipeline>                    ret;
    std::vector<VkComputePipelineCreateInfo> cis;
    ret.reserve(create_info_list.size());
    cis.reserve(create_info_list.size());
    for (auto & ci : create_info_list)
    {
        cis.emplace_back(ci.create_info);
        ret.emplace_back(Pipeline{ std::move(ci.label) });
    }

    std::vector<VkPipeline> vk_pipelines(create_info_list.size(), nullptr);
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateComputePipelines != nullptr)
                                ? opt_args.functions->vkCreateComputePipelines
                                : vkCreateComputePipelines;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyPipeline != nullptr)
                                ? opt_args.functions->vkDestroyPipeline
                                : vkDestroyPipeline;

    const VkResult result = create_fn(device,
                                      opt_args.pipeline_cache != nullptr  //
                                          ? static_cast<VkPipelineCache>(*opt_args.pipeline_cache)
                                          : nullptr,
                                      static_cast<std::uint32_t>(cis.size()),
                                      cis.data(),
                                      nullptr,
                                      vk_pipelines.data());
    for (std::size_t i = 0; i < create_info_list.size(); ++i)
    {
        *ret[i]._reset(destroy_fn, device) = vk_pipelines[i];
    }
    VulkanError::check(result, "Failed to vkCreateComputePipelines().");
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
vkwp::Pipeline::create(const Device &                               device,
                       std::vector<RayTracingPipelineCreateInfoKHR> create_info_list,
                       const RayTracingPipelineKHR_ExtFunctions &   ext_functions)
{
    return Pipeline::create(device, std::move(create_info_list), ext_functions, {});
}

/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                               device,
                       std::vector<RayTracingPipelineCreateInfoKHR> create_info_list,
                       const RayTracingPipelineKHR_ExtFunctions &   ext_functions,
                       const OptionalCreateRTPipelineArgs &         opt_args)
{
    VKWP_VALIDATE_ARG(device);
    std::vector<Pipeline>                          ret;
    std::vector<VkRayTracingPipelineCreateInfoKHR> cis;
    ret.reserve(create_info_list.size());
    cis.reserve(create_info_list.size());
    for (auto & ci : create_info_list)
    {
        cis.emplace_back(ci.create_info);
        ret.emplace_back(Pipeline{ std::move(ci.label) });
    }

    std::vector<VkPipeline> vk_pipelines(create_info_list.size(), nullptr);
    auto * const            create_fn
        = (opt_args.functions != nullptr && opt_args.functions->vkCreateRayTracingPipelinesKHR != nullptr)
            ? opt_args.functions->vkCreateRayTracingPipelinesKHR
            : ext_functions.vkCreateRayTracingPipelinesKHR;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyPipeline != nullptr)
                                ? opt_args.functions->vkDestroyPipeline
                                : vkDestroyPipeline;

    const VkResult result = create_fn(device,
                                      nullptr,                            // *opt_args.deferred_operation,
                                      opt_args.pipeline_cache != nullptr  //
                                          ? static_cast<VkPipelineCache>(*opt_args.pipeline_cache)
                                          : nullptr,
                                      static_cast<std::uint32_t>(cis.size()),
                                      cis.data(),
                                      nullptr,
                                      vk_pipelines.data());
    for (std::size_t i = 0; i < create_info_list.size(); ++i)
    {
        *ret[i]._reset(destroy_fn, device) = vk_pipelines[i];
    }
    VulkanError::check(result, "Failed to vkCreateRayTracingPipelinesKHR().");
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
