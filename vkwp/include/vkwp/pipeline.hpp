#ifndef VKWP_PIPELINE_HPP
#define VKWP_PIPELINE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>
#include <vector>

namespace vkwp {

struct RayTracingPipelineKHR_ExtFunctions
{
    static RayTracingPipelineKHR_ExtFunctions
    create(const VkInstance instance, const VulkanFunctions * functions = nullptr);

    PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR = nullptr;
};

struct GraphicsPipelineCreateInfo
{
    std::string                  label;
    VkGraphicsPipelineCreateInfo create_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stageCount          = 0,
        .pStages             = nullptr,
        .pVertexInputState   = nullptr,
        .pInputAssemblyState = nullptr,
        .pTessellationState  = nullptr,
        .pViewportState      = nullptr,
        .pRasterizationState = nullptr,
        .pMultisampleState   = nullptr,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = nullptr,
        .pDynamicState       = nullptr,
        .layout              = nullptr,
        .renderPass          = nullptr,
        .subpass             = 0,
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = 0,
    };
};

struct ComputePipelineCreateInfo
{
    std::string                 label;
    VkComputePipelineCreateInfo create_info{
        .sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .stage              = {},
        .layout             = nullptr,
        .basePipelineHandle = nullptr,
        .basePipelineIndex  = 0,
    };
};

struct RayTracingPipelineCreateInfoKHR
{
    std::string                       label;
    VkRayTracingPipelineCreateInfoKHR create_info{
        .sType                        = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
        .pNext                        = nullptr,
        .flags                        = 0,
        .stageCount                   = 0,
        .pStages                      = nullptr,
        .groupCount                   = 0,
        .pGroups                      = nullptr,
        .maxPipelineRayRecursionDepth = 0,
        .pLibraryInfo                 = nullptr,
        .pLibraryInterface            = nullptr,
        .pDynamicState                = nullptr,
        .layout                       = nullptr,
        .basePipelineHandle           = nullptr,
        .basePipelineIndex            = 0,
    };
};

///
/// @brief VkPipeline wrapper.
///
class Pipeline final : public Resource<VkPipeline, PFN_vkDestroyPipeline, VkDevice>
{
public:
    using Resource<VkPipeline, PFN_vkDestroyPipeline, VkDevice>::Resource;

    struct OptionalCreateArgs
    {
        const PipelineCache *      pipeline_cache = nullptr;
        const DebugUtilsResource * debug_utils    = nullptr;
        const VulkanFunctions *    functions      = nullptr;
    };

    struct OptionalCreateRTPipelineArgs
    {
        const DeferredOperationKHR * deferred_operation = nullptr;
        const PipelineCache *        pipeline_cache     = nullptr;
        const DebugUtilsResource *   debug_utils        = nullptr;
        const VulkanFunctions *      functions          = nullptr;
    };

    static std::vector<Pipeline>
    create(const Device &                                  device,  //
           const std::vector<GraphicsPipelineCreateInfo> & create_info_list);

    static std::vector<Pipeline>
    create(const Device &                                  device,
           const std::vector<GraphicsPipelineCreateInfo> & create_info_list,
           const OptionalCreateArgs &                      opt_args);

    static std::vector<Pipeline>
    create(const Device &                                 device,  //
           const std::vector<ComputePipelineCreateInfo> & create_info_list);

    static std::vector<Pipeline>
    create(const Device &                                 device,
           const std::vector<ComputePipelineCreateInfo> & create_info_list,
           const OptionalCreateArgs &                     opt_args);

    static std::vector<Pipeline>
    create(const Device &                                       device,
           const std::vector<RayTracingPipelineCreateInfoKHR> & create_info_list,
           const RayTracingPipelineKHR_ExtFunctions &           ext_functions);

    static std::vector<Pipeline>
    create(const Device &                                       device,
           const std::vector<RayTracingPipelineCreateInfoKHR> & create_info_list,
           const RayTracingPipelineKHR_ExtFunctions &           ext_functions,
           const OptionalCreateRTPipelineArgs &                 opt_args);
};

}  // namespace vkwp

#endif
