#include "vkwp/debug_report_callback_resource.hpp"

#include "impl/assert.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace vkwp {

class DebugReportCallbackResource::Impl_ final
{
public:
    Impl_(const VkInstance & instance);
    ~Impl_() noexcept;

    void
    initialize();

private:
    static VkBool32 VKAPI_CALL
    _debugReportCallback(VkDebugReportFlagsEXT      flags,
                         VkDebugReportObjectTypeEXT object_type,
                         std::uint64_t /*object*/,
                         std::size_t /*location*/,
                         std::int32_t /*message_code*/,
                         const char * /*layer_prefix*/,
                         const char * message,
                         void * /*userData*/);

    const VkInstance                          instance_                            = nullptr;
    const PFN_vkCreateDebugReportCallbackEXT  pfn_vkCreateDebugReportCallbackEXT_  = nullptr;
    const PFN_vkDebugReportMessageEXT         pfn_vkDebugReportMessageEXT_         = nullptr;
    const PFN_vkDestroyDebugReportCallbackEXT pfn_vkDestroyDebugReportCallbackEXT_ = nullptr;
    VkDebugReportCallbackEXT                  pfn_debugReport_                     = nullptr;
};

}  // namespace vkwp

// ============================================================================================= //
// Constructor
// ============================================================================================= //
#define GET_INSTANCE_PROC_ADDR(FuncName)                                                    \
    pfn_##FuncName##_                                                                       \
    {                                                                                       \
        reinterpret_cast<PFN_##FuncName>(vkGetInstanceProcAddr(this->instance_, #FuncName)) \
    }

vkwp::DebugReportCallbackResource::Impl_::Impl_(const VkInstance & instance)
    : instance_{ instance }
    , GET_INSTANCE_PROC_ADDR(vkCreateDebugReportCallbackEXT)
    , GET_INSTANCE_PROC_ADDR(vkDebugReportMessageEXT)
    , GET_INSTANCE_PROC_ADDR(vkDestroyDebugReportCallbackEXT)
{}
#undef GET_INSTANCE_PROC_ADDR

// impl_ instance is not created at DebugReportCallbackResource constructor.
vkwp::DebugReportCallbackResource::DebugReportCallbackResource() = default;

// ============================================================================================= //
// Destructor
// ============================================================================================= //
vkwp::DebugReportCallbackResource::Impl_::~Impl_() noexcept
{
    if (this->instance_ != nullptr            //
        && this->pfn_debugReport_ != nullptr  //
        && this->pfn_vkDestroyDebugReportCallbackEXT_ != nullptr)
    {
        this->pfn_vkDestroyDebugReportCallbackEXT_(this->instance_, this->pfn_debugReport_, nullptr);
        this->pfn_debugReport_ = nullptr;
    }
}

vkwp::DebugReportCallbackResource::~DebugReportCallbackResource() noexcept = default;

// ============================================================================================= //
// initialize()
// ============================================================================================= //
void
vkwp::DebugReportCallbackResource::Impl_::initialize()
{
    const VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info_ext{
        .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        .pNext       = nullptr,
        .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
        .pfnCallback = &DebugReportCallbackResource::Impl_::_debugReportCallback,
        .pUserData   = nullptr,
    };
    VulkanError::check(  //
        this->pfn_vkCreateDebugReportCallbackEXT_(this->instance_,
                                                  &debug_report_callback_create_info_ext,
                                                  nullptr,
                                                  &this->pfn_debugReport_),
        "Failed to vkCreateDebugReportCallbackEXT()");
    SPDLOG_DEBUG("Debug callback is setup.");
}

void
vkwp::DebugReportCallbackResource::initialize(const VkInstance instance)
{
    auto impl = std::make_unique<Impl_>(instance);
    impl->initialize();  // throwable なので impl local variable に一度受ける
    this->impl_ = std::move(impl);
}

// ============================================================================================= //
// [IMPL/PRIVATE] _debugReportCallback
// ============================================================================================= //
VkBool32
vkwp::DebugReportCallbackResource::Impl_::_debugReportCallback(VkDebugReportFlagsEXT      flags,
                                                               VkDebugReportObjectTypeEXT object_type,
                                                               std::uint64_t /*object*/,
                                                               std::size_t /*location*/,
                                                               std::int32_t /*message_code*/,
                                                               const char * /*layer_prefix*/,
                                                               const char * message,
                                                               void * /*userData*/)
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
