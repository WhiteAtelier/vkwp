#include "vkwp/debug_report_callback_resource.hpp"

#if VKWP_DEBUG_REPORT_CALLBACK_RESOURCE_ENABLED
#    include "impl/assert.hpp"

#    include <spdlog/spdlog.h>

vkwp::DebugReportCallbackResource::DebugReportCallbackResource() = default;

vkwp::DebugReportCallbackResource::~DebugReportCallbackResource() noexcept
{
    if (this->instance_ != nullptr            //
        && this->pfn_debugReport_ != nullptr  //
        && this->pfn_vkDestroyDebugReportCallbackEXT_ != nullptr)
    {
        this->pfn_vkDestroyDebugReportCallbackEXT_(this->instance_, this->pfn_debugReport_, nullptr);
        this->pfn_debugReport_ = nullptr;
    }
}

void
vkwp::DebugReportCallbackResource::initialize(const VkInstance instance)
{
    this->instance_ = instance;

#    define GET_INSTANCE_PROC_ADDR(FuncName) \
        this->pfn_##FuncName##_ = reinterpret_cast<PFN_##FuncName>(vkGetInstanceProcAddr(this->instance_, #FuncName))
    GET_INSTANCE_PROC_ADDR(vkCreateDebugReportCallbackEXT);
    GET_INSTANCE_PROC_ADDR(vkDebugReportMessageEXT);
    GET_INSTANCE_PROC_ADDR(vkDestroyDebugReportCallbackEXT);
#    undef GET_INSTANCE_PROC_ADDR

    const VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info_ext{
        .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        .pNext       = nullptr,
        .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
        .pfnCallback = &DebugReportCallbackResource::_debugReportCallback,
        .pUserData   = nullptr,
    };
    this->pfn_vkCreateDebugReportCallbackEXT_(this->instance_,
                                              &debug_report_callback_create_info_ext,
                                              nullptr,
                                              &this->pfn_debugReport_);
    SPDLOG_DEBUG("Debug callback is setup.");
}

VkBool32
vkwp::DebugReportCallbackResource::_debugReportCallback(VkDebugReportFlagsEXT      flags,
                                                        VkDebugReportObjectTypeEXT object_type,
                                                        uint64_t                   object,
                                                        size_t                     location,
                                                        int32_t                    message_code,
                                                        const char *               layer_prefix,
                                                        const char *               message,
                                                        void *                     userData)
{
    VkBool32 ret{ VK_FALSE };
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT || flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        ret = VK_TRUE;
    }
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        SPDLOG_ERROR(message);
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        SPDLOG_WARN(message);
    }
    else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        SPDLOG_INFO(message);
    }
    else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        SPDLOG_DEBUG(message);
    }

    SPDLOG_DEBUG("-- ObjectTypeEXT = {}", static_cast<int>(object_type));
    if (object_type != VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT)
    {
        SPDLOG_DEBUG("-- Object        = {}", object);
    }
    else
    {
        SPDLOG_DEBUG("-- Object        = (nodata)");
    }
    SPDLOG_DEBUG("-- Location      = {}", location);
    SPDLOG_DEBUG("-- Message Code  = {}", message_code);
    SPDLOG_DEBUG("-- Layer Prefix  = {}", layer_prefix != nullptr ? layer_prefix : "(nullptr)");

    VKWP_ASSERTION(!(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) && "Vulkan Validation Error!!");

    return ret;
}
#endif
