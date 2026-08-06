#ifndef VKWP_VULKAN_FUNCTIONS_HPP
#define VKWP_VULKAN_FUNCTIONS_HPP

#include <vkwp/vulkan.hpp>

namespace vkwp {

#define VKWP_VULKAN_FUNCTION(fnName) PFN_##fnName fnName = nullptr

/// @brief vkwp が呼び出す Vulkan 関数を指定する構造体.
///
/// 各メンバーに関数ポインタを設定すると, 対応する vkwp API は既定の Vulkan 関数の代わりに
/// 設定された関数を使用します.
/// テストや Vulkan 関数のフックなどに利用できます.
/// nullptr のメンバーについては, 対応する既定の Vulkan 関数が使用されます.
///
/// Resource の create 系関数, allocate 系関数, initialize 関数に渡された場合, 生成, 操作,
/// 破棄など, そのコンテキストに関連するすべての Vulkan 関数が生成時に Resource 内部へ
/// キャッシュされます.
/// そのため, 生成完了後にこの構造体を保持し続ける必要はありません.
/// また, 生成後にメンバーを書き換えても, 生成済み Resource が使用する関数には反映されません.
///
/// @note
///     設定した関数ポインタの参照先は, それをキャッシュした Resource が使用している間,
///     呼び出し可能な状態を維持する必要がある.
///
/// @note
///     この構造体は内部同期を行わない.
///     メンバーを変更しない場合は, 複数スレッドの create 系関数から同時に参照できる.
///     参照中にメンバーを変更してはならない.
///
/// @note
///     関数ポインタのキャッシュは, 生成された Resource や設定された Vulkan 関数の
///     スレッド安全性を保証しない.
///     呼び出し時には Vulkan の外部同期要件と, 設定した関数のスレッド安全性に従う必要がある.
///
struct VulkanFunctions
{
    VKWP_VULKAN_FUNCTION(vkAllocateCommandBuffers);
    VKWP_VULKAN_FUNCTION(vkAllocateDescriptorSets);
    VKWP_VULKAN_FUNCTION(vkAllocateMemory);
    VKWP_VULKAN_FUNCTION(vkBeginCommandBuffer);
    VKWP_VULKAN_FUNCTION(vkBindBufferMemory);
    VKWP_VULKAN_FUNCTION(vkBindImageMemory);
    VKWP_VULKAN_FUNCTION(vkCmdBeginDebugUtilsLabelEXT);
    VKWP_VULKAN_FUNCTION(vkCmdEndDebugUtilsLabelEXT);
    VKWP_VULKAN_FUNCTION(vkCmdInsertDebugUtilsLabelEXT);
    VKWP_VULKAN_FUNCTION(vkCreateAccelerationStructureKHR);
    VKWP_VULKAN_FUNCTION(vkCreateBuffer);
    VKWP_VULKAN_FUNCTION(vkCreateCommandPool);
    VKWP_VULKAN_FUNCTION(vkCreateComputePipelines);
    VKWP_VULKAN_FUNCTION(vkCreateDebugReportCallbackEXT);
    VKWP_VULKAN_FUNCTION(vkCreateDescriptorPool);
    VKWP_VULKAN_FUNCTION(vkCreateDescriptorSetLayout);
    VKWP_VULKAN_FUNCTION(vkCreateDevice);
    VKWP_VULKAN_FUNCTION(vkCreateFence);
    VKWP_VULKAN_FUNCTION(vkCreateFramebuffer);
    VKWP_VULKAN_FUNCTION(vkCreateGraphicsPipelines);
    VKWP_VULKAN_FUNCTION(vkCreateImage);
    VKWP_VULKAN_FUNCTION(vkCreateImageView);
    VKWP_VULKAN_FUNCTION(vkCreateInstance);
    VKWP_VULKAN_FUNCTION(vkCreatePipelineCache);
    VKWP_VULKAN_FUNCTION(vkCreatePipelineLayout);
    VKWP_VULKAN_FUNCTION(vkCreateRayTracingPipelinesKHR);
    VKWP_VULKAN_FUNCTION(vkCreateRenderPass);
    VKWP_VULKAN_FUNCTION(vkCreateSampler);
    VKWP_VULKAN_FUNCTION(vkCreateSemaphore);
    VKWP_VULKAN_FUNCTION(vkCreateShaderModule);
    VKWP_VULKAN_FUNCTION(vkCreateSwapchainKHR);
    VKWP_VULKAN_FUNCTION(vkDebugReportMessageEXT);
    VKWP_VULKAN_FUNCTION(vkDestroyAccelerationStructureKHR);
    VKWP_VULKAN_FUNCTION(vkDestroyBuffer);
    VKWP_VULKAN_FUNCTION(vkDestroyCommandPool);
    VKWP_VULKAN_FUNCTION(vkDestroyDebugReportCallbackEXT);
    VKWP_VULKAN_FUNCTION(vkDestroyDescriptorPool);
    VKWP_VULKAN_FUNCTION(vkDestroyDescriptorSetLayout);
    VKWP_VULKAN_FUNCTION(vkDestroyDevice);
    VKWP_VULKAN_FUNCTION(vkDestroyFence);
    VKWP_VULKAN_FUNCTION(vkDestroyFramebuffer);
    VKWP_VULKAN_FUNCTION(vkDestroyImage);
    VKWP_VULKAN_FUNCTION(vkDestroyImageView);
    VKWP_VULKAN_FUNCTION(vkDestroyInstance);
    VKWP_VULKAN_FUNCTION(vkDestroyPipeline);
    VKWP_VULKAN_FUNCTION(vkDestroyPipelineCache);
    VKWP_VULKAN_FUNCTION(vkDestroyPipelineLayout);
    VKWP_VULKAN_FUNCTION(vkDestroyRenderPass);
    VKWP_VULKAN_FUNCTION(vkDestroySampler);
    VKWP_VULKAN_FUNCTION(vkDestroySemaphore);
    VKWP_VULKAN_FUNCTION(vkDestroyShaderModule);
    VKWP_VULKAN_FUNCTION(vkDestroySurfaceKHR);
    VKWP_VULKAN_FUNCTION(vkDestroySwapchainKHR);
    VKWP_VULKAN_FUNCTION(vkDeviceWaitIdle);
    VKWP_VULKAN_FUNCTION(vkEndCommandBuffer);
    VKWP_VULKAN_FUNCTION(vkEnumerateDeviceExtensionProperties);
    VKWP_VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties);
    VKWP_VULKAN_FUNCTION(vkEnumeratePhysicalDevices);
    VKWP_VULKAN_FUNCTION(vkFreeCommandBuffers);
    VKWP_VULKAN_FUNCTION(vkFreeDescriptorSets);
    VKWP_VULKAN_FUNCTION(vkFreeMemory);
    VKWP_VULKAN_FUNCTION(vkGetBufferDeviceAddress);
    VKWP_VULKAN_FUNCTION(vkGetBufferMemoryRequirements);
    VKWP_VULKAN_FUNCTION(vkGetDeviceProcAddr);
    VKWP_VULKAN_FUNCTION(vkGetDeviceQueue);
    VKWP_VULKAN_FUNCTION(vkGetImageMemoryRequirements);
    VKWP_VULKAN_FUNCTION(vkGetImageMemoryRequirements2);
    VKWP_VULKAN_FUNCTION(vkGetInstanceProcAddr);
    VKWP_VULKAN_FUNCTION(vkGetPhysicalDeviceFormatProperties);
    VKWP_VULKAN_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
    VKWP_VULKAN_FUNCTION(vkGetPhysicalDeviceProperties);
    VKWP_VULKAN_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
    VKWP_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    VKWP_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
    VKWP_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
    VKWP_VULKAN_FUNCTION(vkGetSwapchainImagesKHR);
    VKWP_VULKAN_FUNCTION(vkMapMemory);
    VKWP_VULKAN_FUNCTION(vkQueueBeginDebugUtilsLabelEXT);
    VKWP_VULKAN_FUNCTION(vkQueueEndDebugUtilsLabelEXT);
    VKWP_VULKAN_FUNCTION(vkResetCommandPool);
    VKWP_VULKAN_FUNCTION(vkResetFences);
    VKWP_VULKAN_FUNCTION(vkSetDebugUtilsObjectNameEXT);
    VKWP_VULKAN_FUNCTION(vkSetHdrMetadataEXT);
    VKWP_VULKAN_FUNCTION(vkUnmapMemory);
    VKWP_VULKAN_FUNCTION(vkWaitForFences);

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    VKWP_VULKAN_FUNCTION(vkCreateWin32SurfaceKHR);
    VKWP_VULKAN_FUNCTION(vkGetMemoryWin32HandlePropertiesKHR);
#endif
};

#undef VKWP_VULKAN_FUNCTION

}  // namespace vkwp

#endif
