#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

// ************************************************************************************************
// Constructor
// ************************************************************************************************
vkwp::VulkanError::VulkanError(const VkResult result, const std::string_view message)
    : runtime_error{ VulkanError::_toMessage(result, message) }
    , result_{ result }
{}

// ************************************************************************************************
// result()
// ************************************************************************************************
VkResult
vkwp::VulkanError::result() const noexcept
{
    return static_cast<VkResult>(this->result_);
}

// ************************************************************************************************
// check()
// ************************************************************************************************
/* static */
void
vkwp::VulkanError::check(const VkResult result, const std::string_view message)
{
    if (result != VkResult::VK_SUCCESS)
    {
        SPDLOG_CRITICAL("VulkanError: {}", VulkanError::_toMessage(result, message));
        throw VulkanError{ result, message };
    }
}

// ************************************************************************************************
// [PRIVATE / STATIC] _toMessage()
// ************************************************************************************************
std::string
vkwp::VulkanError::_toMessage(const VkResult result, const std::string_view message)
{
    std::string str{ message };
    str += ", Code = ";

    switch (result)
    {
    case VkResult::VK_SUCCESS:  //
        str += "VK_SUCCESS";
        break;
    case VkResult::VK_NOT_READY:  //
        str += "VK_NOT_READY";
        break;
    case VkResult::VK_TIMEOUT:  //
        str += "VK_TIMEOUT";
        break;
    case VkResult::VK_EVENT_SET:  //
        str += "VK_EVENT_SET";
        break;
    case VkResult::VK_EVENT_RESET:  //
        str += "VK_EVENT_RESET";
        break;
    case VkResult::VK_INCOMPLETE:  //
        str += "VK_INCOMPLETE";
        break;
    case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY:  //
        str += "VK_ERROR_OUT_OF_HOST_MEMORY";
        break;
    case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY:  //
        str += "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        break;
    case VkResult::VK_ERROR_INITIALIZATION_FAILED:  //
        str += "VK_ERROR_INITIALIZATION_FAILED";
        break;
    case VkResult::VK_ERROR_DEVICE_LOST:  //
        str += "VK_ERROR_DEVICE_LOST";
        break;
    case VkResult::VK_ERROR_MEMORY_MAP_FAILED:  //
        str += "VK_ERROR_MEMORY_MAP_FAILED";
        break;
    case VkResult::VK_ERROR_LAYER_NOT_PRESENT:  //
        str += "VK_ERROR_LAYER_NOT_PRESENT";
        break;
    case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:  //
        str += "VK_ERROR_EXTENSION_NOT_PRESENT";
        break;
    case VkResult::VK_ERROR_FEATURE_NOT_PRESENT:  //
        str += "VK_ERROR_FEATURE_NOT_PRESENT";
        break;
    case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:  //
        str += "VK_ERROR_INCOMPATIBLE_DRIVER";
        break;
    case VkResult::VK_ERROR_TOO_MANY_OBJECTS:  //
        str += "VK_ERROR_TOO_MANY_OBJECTS";
        break;
    case VkResult::VK_ERROR_FORMAT_NOT_SUPPORTED:  //
        str += "VK_ERROR_FORMAT_NOT_SUPPORTED";
        break;
    case VkResult::VK_ERROR_FRAGMENTED_POOL:  //
        str += "VK_ERROR_FRAGMENTED_POOL";
        break;
    case VkResult::VK_ERROR_UNKNOWN:  //
        str += "VK_ERROR_UNKNOWN";
        break;
    case VkResult::VK_ERROR_OUT_OF_POOL_MEMORY:  //
        str += "VK_ERROR_OUT_OF_POOL_MEMORY";
        break;
    case VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE:  //
        str += "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        break;
    case VkResult::VK_ERROR_FRAGMENTATION:  //
        str += "VK_ERROR_FRAGMENTATION";
        break;
    case VkResult::VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:  //
        str += "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        break;
    case VkResult::VK_ERROR_SURFACE_LOST_KHR:  //
        str += "VK_ERROR_SURFACE_LOST_KHR";
        break;
    case VkResult::VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:  //
        str += "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        break;
    case VkResult::VK_SUBOPTIMAL_KHR:  //
        str += "VK_SUBOPTIMAL_KHR";
        break;
    case VkResult::VK_ERROR_OUT_OF_DATE_KHR:  //
        str += "VK_ERROR_OUT_OF_DATE_KHR";
        break;
    case VkResult::VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:  //
        str += "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        break;
    case VkResult::VK_ERROR_VALIDATION_FAILED_EXT:  //
        str += "VK_ERROR_VALIDATION_FAILED_EXT";
        break;
    case VkResult::VK_ERROR_INVALID_SHADER_NV:  //
        str += "VK_ERROR_INVALID_SHADER_NV";
        break;
    case VkResult::VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        str += "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        break;
    case VkResult::VK_ERROR_NOT_PERMITTED_EXT:  //
        str += "VK_ERROR_NOT_PERMITTED_EXT";
        break;
    case VkResult::VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:  //
        str += "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        break;
    case VkResult::VK_THREAD_IDLE_KHR:  //
        str += "VK_THREAD_IDLE_KHR";
        break;
    case VkResult::VK_THREAD_DONE_KHR:  //
        str += "VK_THREAD_DONE_KHR";
        break;
    case VkResult::VK_OPERATION_DEFERRED_KHR:  //
        str += "VK_OPERATION_DEFERRED_KHR";
        break;
    case VkResult::VK_OPERATION_NOT_DEFERRED_KHR:  //
        str += "VK_OPERATION_NOT_DEFERRED_KHR";
        break;
    case VkResult::VK_PIPELINE_COMPILE_REQUIRED_EXT:  //
        str += "VK_PIPELINE_COMPILE_REQUIRED_EXT";
        break;
    default: str += "(Unknown result code " + std::to_string(static_cast<int>(result)) + ")"; break;
    }
    return str;
}
