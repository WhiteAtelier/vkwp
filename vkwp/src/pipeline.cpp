#include "vkwp/pipeline.hpp"

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
vkwp::impl::_delete<VkPipeline, VkDevice>(VkPipeline i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyPipeline(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Pipeline class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                  device,
                       const std::vector<GraphicsPipelineCreateInfo> & create_info_list,
                       const OptionalCreateArgs &                      opt_args)
{
    VKWP_VALIDATE_ARG(device);
    std::vector<Pipeline> ret;
    VulkanError::check(  //
        vkCreateGraphicsPipelines(this->_ref(), nullptr, 1, &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateGraphicsPipelines().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                 device,
                       const std::vector<ComputePipelineCreateInfo> & create_info_list,
                       const OptionalCreateArgs &                     opt_args)
{
    return std::vector<Pipeline>();
}

std::vector<vkwp::Pipeline>
vkwp::Pipeline::create(const Device &                                       device,
                       const std::vector<RayTracingPipelineCreateInfoKHR> & create_info_list,
                       const OptionalCreateRTPipelineArgs &                 opt_args)
{
    return std::vector<Pipeline>();
}
