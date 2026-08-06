#ifndef VKWP_VULKAN_FUNCTIONS_HPP
#define VKWP_VULKAN_FUNCTIONS_HPP

#include <vkwp/vulkan.hpp>

namespace vkwp {

#define VKWP_VULKAN_FUNCTION(fnName) PFN_##fnName fnName = nullptr

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
