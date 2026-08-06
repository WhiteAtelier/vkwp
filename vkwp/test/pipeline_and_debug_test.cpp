#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <vkwp/acceleration_structure_khr.hpp>
#include <vkwp/buffer.hpp>
#include <vkwp/debug_report_callback_resource.hpp>
#include <vkwp/debug_utils_resource.hpp>
#include <vkwp/descriptor_set_layout.hpp>
#include <vkwp/device.hpp>
#include <vkwp/pipeline.hpp>
#include <vkwp/pipeline_cache.hpp>
#include <vkwp/pipeline_layout.hpp>
#include <vkwp/surface_khr.hpp>
#include <vkwp/swapchain_khr.hpp>
#include <vkwp/vulkan_functions.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace {

using vkwp::test::fakeHandle;
using vkwp::test::fakePointer;

struct PipelineState
{
    VkDevice                  device                = fakeHandle<VkDevice>(0x3001);
    VkDescriptorSetLayout     descriptor_set_layout = fakeHandle<VkDescriptorSetLayout>(0x3002);
    VkPipelineLayout          pipeline_layout       = fakeHandle<VkPipelineLayout>(0x3003);
    VkPipelineCache           pipeline_cache        = fakeHandle<VkPipelineCache>(0x3004);
    std::array<VkPipeline, 3> pipelines{
        fakeHandle<VkPipeline>(0x3005),
        fakeHandle<VkPipeline>(0x3006),
        fakeHandle<VkPipeline>(0x3007),
    };
    int next_pipeline          = 0;
    int destroy_pipeline_calls = 0;
    int destroy_cache_calls    = 0;
    int destroy_layout_calls   = 0;
};

PipelineState pipeline_state;

VkResult VKAPI_CALL
createPipelineCache(const VkDevice                    device,
                    const VkPipelineCacheCreateInfo * create_info,
                    const VkAllocationCallbacks *     allocator,
                    VkPipelineCache *                 pipeline_cache)
{
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x3101));
    EXPECT_EQ(create_info->flags, 3u);
    EXPECT_EQ(create_info->initialDataSize, 4u);
    EXPECT_EQ(*static_cast<const std::uint32_t *>(create_info->pInitialData), 0x12345678u);
    *pipeline_cache = pipeline_state.pipeline_cache;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyPipelineCache(const VkDevice                device,
                     const VkPipelineCache         pipeline_cache,
                     const VkAllocationCallbacks * allocator)
{
    ++pipeline_state.destroy_cache_calls;
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(pipeline_cache, pipeline_state.pipeline_cache);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createPipelineLayout(const VkDevice                     device,
                     const VkPipelineLayoutCreateInfo * create_info,
                     const VkAllocationCallbacks *      allocator,
                     VkPipelineLayout *                 pipeline_layout)
{
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x3102));
    EXPECT_EQ(create_info->flags, 5u);
    EXPECT_EQ(create_info->setLayoutCount, 1u);
    EXPECT_EQ(create_info->pSetLayouts[0], pipeline_state.descriptor_set_layout);
    EXPECT_EQ(create_info->pushConstantRangeCount, 1u);
    EXPECT_EQ(create_info->pPushConstantRanges[0].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_EQ(create_info->pPushConstantRanges[0].offset, 16u);
    EXPECT_EQ(create_info->pPushConstantRanges[0].size, 32u);
    *pipeline_layout = pipeline_state.pipeline_layout;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyPipelineLayout(const VkDevice                device,
                      const VkPipelineLayout        pipeline_layout,
                      const VkAllocationCallbacks * allocator)
{
    ++pipeline_state.destroy_layout_calls;
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(pipeline_layout, pipeline_state.pipeline_layout);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
createGraphicsPipelines(const VkDevice                       device,
                        const VkPipelineCache                pipeline_cache,
                        const std::uint32_t                  count,
                        const VkGraphicsPipelineCreateInfo * create_infos,
                        const VkAllocationCallbacks *        allocator,
                        VkPipeline *                         pipelines)
{
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(pipeline_cache, pipeline_state.pipeline_cache);
    EXPECT_EQ(count, 1u);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_infos[0].flags, VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT);
    EXPECT_EQ(create_infos[0].layout, pipeline_state.pipeline_layout);
    pipelines[0] = pipeline_state.pipelines[static_cast<std::size_t>(pipeline_state.next_pipeline++)];
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
createComputePipelines(const VkDevice                      device,
                       const VkPipelineCache               pipeline_cache,
                       const std::uint32_t                 count,
                       const VkComputePipelineCreateInfo * create_infos,
                       const VkAllocationCallbacks *       allocator,
                       VkPipeline *                        pipelines)
{
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(pipeline_cache, pipeline_state.pipeline_cache);
    EXPECT_EQ(count, 1u);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_infos[0].flags, VK_PIPELINE_CREATE_DERIVATIVE_BIT);
    EXPECT_EQ(create_infos[0].layout, pipeline_state.pipeline_layout);
    pipelines[0] = pipeline_state.pipelines[static_cast<std::size_t>(pipeline_state.next_pipeline++)];
    return VK_SUCCESS;
}

VkResult VKAPI_CALL
createRayTracingPipelines(const VkDevice                            device,
                          const VkDeferredOperationKHR              deferred_operation,
                          const VkPipelineCache                     pipeline_cache,
                          const std::uint32_t                       count,
                          const VkRayTracingPipelineCreateInfoKHR * create_infos,
                          const VkAllocationCallbacks *             allocator,
                          VkPipeline *                              pipelines)
{
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(deferred_operation, nullptr);
    EXPECT_EQ(pipeline_cache, pipeline_state.pipeline_cache);
    EXPECT_EQ(count, 1u);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_infos[0].maxPipelineRayRecursionDepth, 2u);
    EXPECT_EQ(create_infos[0].layout, pipeline_state.pipeline_layout);
    pipelines[0] = pipeline_state.pipelines[static_cast<std::size_t>(pipeline_state.next_pipeline++)];
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyPipeline(const VkDevice device, const VkPipeline pipeline, const VkAllocationCallbacks * allocator)
{
    ++pipeline_state.destroy_pipeline_calls;
    EXPECT_EQ(device, pipeline_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_TRUE(pipeline == pipeline_state.pipelines[0] || pipeline == pipeline_state.pipelines[1]
                || pipeline == pipeline_state.pipelines[2]);
}

struct SwapchainState
{
    VkDevice               device    = fakeHandle<VkDevice>(0x3201);
    VkSurfaceKHR           surface   = fakeHandle<VkSurfaceKHR>(0x3202);
    VkSwapchainKHR         old_chain = fakeHandle<VkSwapchainKHR>(0x3203);
    VkSwapchainKHR         swapchain = fakeHandle<VkSwapchainKHR>(0x3204);
    std::array<VkImage, 2> images{ fakeHandle<VkImage>(0x3205), fakeHandle<VkImage>(0x3206) };
    int                    destroy_calls = 0;
    int                    image_calls   = 0;
    int                    hdr_calls     = 0;
};

SwapchainState swapchain_state;

VkResult VKAPI_CALL
createSwapchain(const VkDevice                   device,
                const VkSwapchainCreateInfoKHR * create_info,
                const VkAllocationCallbacks *    allocator,
                VkSwapchainKHR *                 swapchain)
{
    EXPECT_EQ(device, swapchain_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->surface, swapchain_state.surface);
    EXPECT_EQ(create_info->minImageCount, 3u);
    EXPECT_EQ(create_info->imageFormat, VK_FORMAT_B8G8R8A8_UNORM);
    EXPECT_EQ(create_info->imageColorSpace, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    EXPECT_EQ(create_info->imageExtent.width, 1280u);
    EXPECT_EQ(create_info->imageExtent.height, 720u);
    EXPECT_EQ(create_info->imageArrayLayers, 2u);
    EXPECT_EQ(create_info->imageUsage, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    EXPECT_EQ(create_info->presentMode, VK_PRESENT_MODE_MAILBOX_KHR);
    EXPECT_EQ(create_info->preTransform, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
    EXPECT_EQ(create_info->oldSwapchain, swapchain_state.old_chain);
    *swapchain = swapchain_state.swapchain;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroySwapchain(const VkDevice device, const VkSwapchainKHR swapchain, const VkAllocationCallbacks * allocator)
{
    ++swapchain_state.destroy_calls;
    EXPECT_EQ(device, swapchain_state.device);
    EXPECT_EQ(swapchain, swapchain_state.swapchain);
    EXPECT_EQ(allocator, nullptr);
}

VkResult VKAPI_CALL
getSwapchainImages(const VkDevice device, const VkSwapchainKHR swapchain, std::uint32_t * count, VkImage * images)
{
    ++swapchain_state.image_calls;
    EXPECT_EQ(device, swapchain_state.device);
    EXPECT_EQ(swapchain, swapchain_state.swapchain);
    if (images == nullptr)
    {
        *count = 2;
    }
    else
    {
        EXPECT_EQ(*count, 2u);
        images[0] = swapchain_state.images[0];
        images[1] = swapchain_state.images[1];
    }
    return VK_SUCCESS;
}

void VKAPI_CALL
setHdrMetadata(const VkDevice           device,
               const std::uint32_t      swapchain_count,
               const VkSwapchainKHR *   swapchains,
               const VkHdrMetadataEXT * metadata)
{
    ++swapchain_state.hdr_calls;
    EXPECT_EQ(device, swapchain_state.device);
    EXPECT_EQ(swapchain_count, 1u);
    EXPECT_EQ(swapchains[0], swapchain_state.swapchain);
    EXPECT_FLOAT_EQ(metadata->maxLuminance, 1000.0f);
    EXPECT_FLOAT_EQ(metadata->minLuminance, 0.1f);
}

struct AccelerationState
{
    VkDevice                   device         = fakeHandle<VkDevice>(0x3301);
    VkBuffer                   buffer         = fakeHandle<VkBuffer>(0x3302);
    VkAccelerationStructureKHR acceleration   = fakeHandle<VkAccelerationStructureKHR>(0x3303);
    int                        get_proc_calls = 0;
    int                        create_calls   = 0;
    int                        destroy_calls  = 0;
};

AccelerationState acceleration_state;

VkResult VKAPI_CALL
createAccelerationStructure(const VkDevice                               device,
                            const VkAccelerationStructureCreateInfoKHR * create_info,
                            const VkAllocationCallbacks *                allocator,
                            VkAccelerationStructureKHR *                 acceleration_structure)
{
    ++acceleration_state.create_calls;
    EXPECT_EQ(device, acceleration_state.device);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->pNext, fakePointer(0x3304));
    EXPECT_EQ(create_info->buffer, acceleration_state.buffer);
    EXPECT_EQ(create_info->offset, 256u);
    EXPECT_EQ(create_info->size, 4096u);
    EXPECT_EQ(create_info->type, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);
    EXPECT_EQ(create_info->deviceAddress, 0x9876u);
    *acceleration_structure = acceleration_state.acceleration;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyAccelerationStructure(const VkDevice                   device,
                             const VkAccelerationStructureKHR acceleration_structure,
                             const VkAllocationCallbacks *    allocator)
{
    ++acceleration_state.destroy_calls;
    EXPECT_EQ(device, acceleration_state.device);
    EXPECT_EQ(acceleration_structure, acceleration_state.acceleration);
    EXPECT_EQ(allocator, nullptr);
}

PFN_vkVoidFunction VKAPI_CALL
getDeviceProcAddr(const VkDevice device, const char * name)
{
    ++acceleration_state.get_proc_calls;
    EXPECT_EQ(device, acceleration_state.device);
    if (std::string_view{ name } == "vkCreateAccelerationStructureKHR")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&createAccelerationStructure);
    }
    if (std::string_view{ name } == "vkDestroyAccelerationStructureKHR")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&destroyAccelerationStructure);
    }
    return nullptr;
}

struct DebugState
{
    VkInstance                   instance             = fakeHandle<VkInstance>(0x3401);
    VkDevice                     device               = fakeHandle<VkDevice>(0x3402);
    VkDebugReportCallbackEXT     callback             = fakeHandle<VkDebugReportCallbackEXT>(0x3403);
    PFN_vkDebugReportCallbackEXT callback_fn          = nullptr;
    void *                       user_data            = nullptr;
    int                          get_proc_calls       = 0;
    int                          create_report_calls  = 0;
    int                          destroy_report_calls = 0;
    int                          object_name_calls    = 0;
};

DebugState debug_state;

class RecordingDebugReportCallback final : public vkwp::DebugReportCallback
{
public:
    void
    operator()(const VkDebugReportFlagsEXT      flags,
               const VkDebugReportObjectTypeEXT object_type,
               const std::uint64_t              object,
               const std::size_t                location,
               const std::int32_t               message_code,
               const char *                     layer_prefix,
               const char *                     message) override
    {
        ++call_count;
        received_flags        = flags;
        received_object_type  = object_type;
        received_object       = object;
        received_location     = location;
        received_message_code = message_code;
        received_layer_prefix = layer_prefix != nullptr ? layer_prefix : "";
        received_message      = message != nullptr ? message : "";
    }

    int                        call_count            = 0;
    VkDebugReportFlagsEXT      received_flags        = 0;
    VkDebugReportObjectTypeEXT received_object_type  = VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT;
    std::uint64_t              received_object       = 0;
    std::size_t                received_location     = 0;
    std::int32_t               received_message_code = 0;
    std::string                received_layer_prefix;
    std::string                received_message;
};

VkResult VKAPI_CALL
createDebugReport(const VkInstance                           instance,
                  const VkDebugReportCallbackCreateInfoEXT * create_info,
                  const VkAllocationCallbacks *              allocator,
                  VkDebugReportCallbackEXT *                 callback)
{
    ++debug_state.create_report_calls;
    EXPECT_EQ(instance, debug_state.instance);
    EXPECT_EQ(allocator, nullptr);
    EXPECT_EQ(create_info->flags, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT);
    EXPECT_NE(create_info->pfnCallback, nullptr);
    EXPECT_NE(create_info->pUserData, nullptr);
    debug_state.callback_fn = create_info->pfnCallback;
    debug_state.user_data   = create_info->pUserData;
    *callback = debug_state.callback;
    return VK_SUCCESS;
}

void VKAPI_CALL
destroyDebugReport(const VkInstance               instance,
                   const VkDebugReportCallbackEXT callback,
                   const VkAllocationCallbacks *  allocator)
{
    ++debug_state.destroy_report_calls;
    EXPECT_EQ(instance, debug_state.instance);
    EXPECT_EQ(callback, debug_state.callback);
    EXPECT_EQ(allocator, nullptr);
}

void VKAPI_CALL
debugReportMessage(const VkInstance,
                   const VkDebugReportFlagsEXT,
                   const VkDebugReportObjectTypeEXT,
                   const std::uint64_t,
                   const std::size_t,
                   const std::int32_t,
                   const char *,
                   const char *)
{}

VkResult VKAPI_CALL
setDebugUtilsObjectName(const VkDevice device, const VkDebugUtilsObjectNameInfoEXT * name_info)
{
    ++debug_state.object_name_calls;
    EXPECT_EQ(device, debug_state.device);
    EXPECT_EQ(name_info->objectType, VK_OBJECT_TYPE_BUFFER);
    EXPECT_EQ(name_info->objectHandle, 0x3404u);
    EXPECT_STREQ(name_info->pObjectName, "mock-buffer");
    return VK_SUCCESS;
}

void VKAPI_CALL
queueBeginLabel(const VkQueue, const VkDebugUtilsLabelEXT *)
{}

void VKAPI_CALL
queueEndLabel(const VkQueue)
{}

void VKAPI_CALL
cmdBeginLabel(const VkCommandBuffer, const VkDebugUtilsLabelEXT *)
{}

void VKAPI_CALL
cmdEndLabel(const VkCommandBuffer)
{}

void VKAPI_CALL
cmdInsertLabel(const VkCommandBuffer, const VkDebugUtilsLabelEXT *)
{}

PFN_vkVoidFunction VKAPI_CALL
getInstanceProcAddr(const VkInstance instance, const char * name)
{
    ++debug_state.get_proc_calls;
    EXPECT_EQ(instance, debug_state.instance);
    const std::string_view function_name{ name };
    if (function_name == "vkDestroyDebugReportCallbackEXT")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&destroyDebugReport);
    }
    if (function_name == "vkDebugReportMessageEXT")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&debugReportMessage);
    }
    if (function_name == "vkQueueBeginDebugUtilsLabelEXT")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&queueBeginLabel);
    }
    if (function_name == "vkQueueEndDebugUtilsLabelEXT")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&queueEndLabel);
    }
    if (function_name == "vkCmdBeginDebugUtilsLabelEXT")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&cmdBeginLabel);
    }
    if (function_name == "vkCmdEndDebugUtilsLabelEXT")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&cmdEndLabel);
    }
    if (function_name == "vkCmdInsertDebugUtilsLabelEXT")
    {
        return reinterpret_cast<PFN_vkVoidFunction>(&cmdInsertLabel);
    }
    return nullptr;
}

}  // namespace

TEST(VulkanFunctionsPipelineTest, PipelineCacheLayoutAndAllPipelineKindsForwardAndDestroy)
{
    pipeline_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreatePipelineCache          = &createPipelineCache;
    functions.vkDestroyPipelineCache         = &destroyPipelineCache;
    functions.vkCreatePipelineLayout         = &createPipelineLayout;
    functions.vkDestroyPipelineLayout        = &destroyPipelineLayout;
    functions.vkCreateGraphicsPipelines      = &createGraphicsPipelines;
    functions.vkCreateComputePipelines       = &createComputePipelines;
    functions.vkCreateRayTracingPipelinesKHR = &createRayTracingPipelines;
    functions.vkDestroyPipeline              = &destroyPipeline;
    const vkwp::Device              device{ "device", pipeline_state.device };
    const vkwp::DescriptorSetLayout set_layout{ "set-layout", pipeline_state.descriptor_set_layout };
    const std::uint32_t             cache_data = 0x12345678u;

    {
        const vkwp::PipelineCache::OptionalCreateArgs cache_args{
            .p_next            = fakePointer(0x3101),
            .flags             = 3,
            .initial_data_size = sizeof(cache_data),
            .initial_data      = &cache_data,
            .debug_utils       = nullptr,
            .functions         = &functions,
        };
        auto                      cache = vkwp::PipelineCache::create("cache", device, cache_args);
        const VkPushConstantRange push_constant{
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .offset     = 16,
            .size       = 32,
        };
        const vkwp::PipelineLayout::OptionalCreateArgs layout_args{
            .p_next      = fakePointer(0x3102),
            .flags       = 5,
            .debug_utils = nullptr,
            .functions   = &functions,
        };
        auto layout
            = vkwp::PipelineLayout::create("layout", device, { std::cref(set_layout) }, { push_constant }, layout_args);

        vkwp::GraphicsPipelineCreateInfo graphics_info{ .label = "graphics" };
        graphics_info.create_info.flags  = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
        graphics_info.create_info.layout = layout;
        const vkwp::Pipeline::OptionalCreateArgs pipeline_args{
            .pipeline_cache = &cache,
            .debug_utils    = nullptr,
            .functions      = &functions,
        };
        auto graphics = vkwp::Pipeline::create(device, { graphics_info }, pipeline_args);

        vkwp::ComputePipelineCreateInfo compute_info{ .label = "compute" };
        compute_info.create_info.flags  = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
        compute_info.create_info.layout = layout;
        auto compute                    = vkwp::Pipeline::create(device, { compute_info }, pipeline_args);

        vkwp::RayTracingPipelineCreateInfoKHR ray_tracing_info{ .label = "ray-tracing" };
        ray_tracing_info.create_info.maxPipelineRayRecursionDepth = 2;
        ray_tracing_info.create_info.layout                       = layout;
        const vkwp::RayTracingPipelineKHR_ExtFunctions     extension_functions{};
        const vkwp::Pipeline::OptionalCreateRTPipelineArgs ray_tracing_args{
            .pipeline_cache = &cache,
            .debug_utils    = nullptr,
            .functions      = &functions,
        };
        auto ray_tracing = vkwp::Pipeline::create(device, { ray_tracing_info }, extension_functions, ray_tracing_args);

        ASSERT_EQ(graphics.size(), 1u);
        ASSERT_EQ(compute.size(), 1u);
        ASSERT_EQ(ray_tracing.size(), 1u);
        EXPECT_EQ(static_cast<VkPipeline>(graphics[0]), pipeline_state.pipelines[0]);
        EXPECT_EQ(static_cast<VkPipeline>(compute[0]), pipeline_state.pipelines[1]);
        EXPECT_EQ(static_cast<VkPipeline>(ray_tracing[0]), pipeline_state.pipelines[2]);
    }
    EXPECT_EQ(pipeline_state.next_pipeline, 3);
    EXPECT_EQ(pipeline_state.destroy_pipeline_calls, 3);
    EXPECT_EQ(pipeline_state.destroy_layout_calls, 1);
    EXPECT_EQ(pipeline_state.destroy_cache_calls, 1);
}

TEST(VulkanFunctionsSwapchainTest, CreateOperationsAndDestroyForwardAllArguments)
{
    swapchain_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateSwapchainKHR    = &createSwapchain;
    functions.vkDestroySwapchainKHR   = &destroySwapchain;
    functions.vkGetSwapchainImagesKHR = &getSwapchainImages;
    functions.vkSetHdrMetadataEXT     = &setHdrMetadata;
    const vkwp::Device       device{ "device", swapchain_state.device };
    const vkwp::SurfaceKHR   surface{ "surface", swapchain_state.surface };
    const vkwp::SwapchainKHR old_swapchain{ "old", swapchain_state.old_chain };

    {
        const vkwp::SwapchainKHR::OptionalCreateArgs optional_args{
            .image_array_layers = 2,
            .old_swapchain      = &old_swapchain,
            .functions          = &functions,
        };
        auto       swapchain = vkwp::SwapchainKHR::create("swapchain",
                                                    device,
                                                    surface,
                                                    3,
                                                    VK_FORMAT_B8G8R8A8_UNORM,
                                                    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                                                    VkExtent2D{ 1280, 720 },
                                                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                    VK_PRESENT_MODE_MAILBOX_KHR,
                                                    VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                                                    optional_args);
        const auto images    = swapchain.getSwapchainImages();
        EXPECT_EQ(images, std::vector<VkImage>(swapchain_state.images.begin(), swapchain_state.images.end()));
        VkHdrMetadataEXT metadata{
            .sType        = VK_STRUCTURE_TYPE_HDR_METADATA_EXT,
            .maxLuminance = 1000.0f,
            .minLuminance = 0.1f,
        };
        swapchain.setHdrMetadata(metadata);
    }
    EXPECT_EQ(swapchain_state.image_calls, 2);
    EXPECT_EQ(swapchain_state.hdr_calls, 1);
    EXPECT_EQ(swapchain_state.destroy_calls, 1);
}

TEST(VulkanFunctionsAccelerationStructureTest, MissingFunctionsResolveThroughDeviceProcAddrAndDestroy)
{
    acceleration_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkGetDeviceProcAddr = &getDeviceProcAddr;
    const vkwp::Device device{ "device", acceleration_state.device };
    const vkwp::Buffer buffer{ "buffer", acceleration_state.buffer };
    {
        const vkwp::AccelerationStructureKHR::OptionalCreateArgs optional_args{
            .p_next         = const_cast<void *>(fakePointer(0x3304)),
            .create_flags   = 0,
            .device_address = 0x9876,
            .debug_utils    = nullptr,
            .functions      = &functions,
        };
        const auto acceleration_structure
            = vkwp::AccelerationStructureKHR::create("acceleration",
                                                     device,
                                                     buffer,
                                                     256,
                                                     4096,
                                                     VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
                                                     optional_args);
        EXPECT_EQ(static_cast<VkAccelerationStructureKHR>(acceleration_structure), acceleration_state.acceleration);
    }
    EXPECT_EQ(acceleration_state.get_proc_calls, 2);
    EXPECT_EQ(acceleration_state.create_calls, 1);
    EXPECT_EQ(acceleration_state.destroy_calls, 1);
}

TEST(VulkanFunctionsAccelerationStructureTest, ProvidedFunctionsAreUsedWithoutDeviceProcAddr)
{
    acceleration_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkCreateAccelerationStructureKHR  = &createAccelerationStructure;
    functions.vkDestroyAccelerationStructureKHR = &destroyAccelerationStructure;
    const vkwp::Device device{ "device", acceleration_state.device };
    const vkwp::Buffer buffer{ "buffer", acceleration_state.buffer };
    {
        const vkwp::AccelerationStructureKHR::OptionalCreateArgs optional_args{
            .p_next         = const_cast<void *>(fakePointer(0x3304)),
            .create_flags   = 0,
            .device_address = 0x9876,
            .debug_utils    = nullptr,
            .functions      = &functions,
        };
        const auto acceleration_structure
            = vkwp::AccelerationStructureKHR::create("acceleration",
                                                     device,
                                                     buffer,
                                                     256,
                                                     4096,
                                                     VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
                                                     optional_args);
        EXPECT_EQ(static_cast<VkAccelerationStructureKHR>(acceleration_structure), acceleration_state.acceleration);
    }
    EXPECT_EQ(acceleration_state.get_proc_calls, 0);
    EXPECT_EQ(acceleration_state.create_calls, 1);
    EXPECT_EQ(acceleration_state.destroy_calls, 1);
}

TEST(VulkanFunctionsDebugTest, DebugReportUsesProvidedFunctionsAndResolvesOnlyMissingOnes)
{
    debug_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkGetInstanceProcAddr          = &getInstanceProcAddr;
    functions.vkCreateDebugReportCallbackEXT = &createDebugReport;
    auto callback                             = std::make_shared<RecordingDebugReportCallback>();
    const auto * const callback_ptr           = callback.get();
    const std::weak_ptr<RecordingDebugReportCallback> weak_callback = callback;
    {
        vkwp::DebugReportCallbackResource debug_report;
        debug_report.initialize(debug_state.instance, callback, &functions);
        callback.reset();

        EXPECT_EQ(debug_state.create_report_calls, 1);
        EXPECT_EQ(debug_state.destroy_report_calls, 0);
        EXPECT_EQ(debug_state.get_proc_calls, 2);
        EXPECT_FALSE(weak_callback.expired());
        ASSERT_NE(debug_state.callback_fn, nullptr);
        ASSERT_NE(debug_state.user_data, nullptr);

        constexpr VkDebugReportFlagsEXT      flags        = VK_DEBUG_REPORT_WARNING_BIT_EXT;
        constexpr VkDebugReportObjectTypeEXT object_type  = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
        constexpr std::uint64_t              object       = 0x3410;
        constexpr std::size_t                location     = 0x3411;
        constexpr std::int32_t               message_code = -3412;
        EXPECT_EQ(debug_state.callback_fn(flags,
                                          object_type,
                                          object,
                                          location,
                                          message_code,
                                          "validation-layer",
                                          "validation-message",
                                          debug_state.user_data),
                  VK_FALSE);
        EXPECT_EQ(callback_ptr->call_count, 1);
        EXPECT_EQ(callback_ptr->received_flags, flags);
        EXPECT_EQ(callback_ptr->received_object_type, object_type);
        EXPECT_EQ(callback_ptr->received_object, object);
        EXPECT_EQ(callback_ptr->received_location, location);
        EXPECT_EQ(callback_ptr->received_message_code, message_code);
        EXPECT_EQ(callback_ptr->received_layer_prefix, "validation-layer");
        EXPECT_EQ(callback_ptr->received_message, "validation-message");
    }
    EXPECT_EQ(debug_state.destroy_report_calls, 1);
    EXPECT_TRUE(weak_callback.expired());
}

TEST(VulkanFunctionsDebugTest, DebugUtilsUsesProvidedFunctionAndResolvesRemainingFunctions)
{
    debug_state = {};
    vkwp::VulkanFunctions functions;
    functions.vkGetInstanceProcAddr        = &getInstanceProcAddr;
    functions.vkSetDebugUtilsObjectNameEXT = &setDebugUtilsObjectName;
    vkwp::DebugUtilsResource debug_utils;
    debug_utils.initialize(debug_state.instance, &functions);
    debug_utils.registDebugUtilsObjectName(debug_state.device,
                                           VK_OBJECT_TYPE_BUFFER,
                                           fakeHandle<VkBuffer>(0x3404),
                                           "mock-buffer");
    EXPECT_EQ(debug_state.get_proc_calls, 5);
    EXPECT_EQ(debug_state.object_name_calls, 1);
}
