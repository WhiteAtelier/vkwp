// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "vkwp/debug_report_callback_resource.hpp"

#include "impl/assert.hpp"

#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace vkwp {

class DebugReportCallbackResource::Impl_ final
{
public:
    Impl_(const VkInstance &                      instance,
          std::shared_ptr<DebugReportCallback> && callback,
          const VulkanFunctions *                 functions);
    ~Impl_() noexcept;

    void
    initialize();

private:
    static VkBool32 VKAPI_CALL
    _debugReportCallback(VkDebugReportFlagsEXT      flags,
                         VkDebugReportObjectTypeEXT object_type,
                         std::uint64_t              object,
                         std::size_t                location,
                         std::int32_t               message_code,
                         const char *               layer_prefix,
                         const char *               message,
                         void *                     user_data);

    const VkInstance                     instance_                        = nullptr;
    PFN_vkCreateDebugReportCallbackEXT   _vkCreateDebugReportCallbackEXT  = nullptr;
    PFN_vkDebugReportMessageEXT          _vkDebugReportMessageEXT         = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT  _vkDestroyDebugReportCallbackEXT = nullptr;
    VkDebugReportCallbackEXT             _debugReport                     = nullptr;
    std::shared_ptr<DebugReportCallback> callback_;
};

}  // namespace vkwp

// ============================================================================================= //
// Constructor
// ============================================================================================= //
vkwp::DebugReportCallbackResource::Impl_::Impl_(const VkInstance &                      instance,
                                                std::shared_ptr<DebugReportCallback> && callback,
                                                const VulkanFunctions *                 functions)
    : instance_{ instance }
    , callback_{ std::move(callback) }
{
    if (functions != nullptr)
    {
        this->_vkCreateDebugReportCallbackEXT  = functions->vkCreateDebugReportCallbackEXT;
        this->_vkDebugReportMessageEXT         = functions->vkDebugReportMessageEXT;
        this->_vkDestroyDebugReportCallbackEXT = functions->vkDestroyDebugReportCallbackEXT;
    }

    auto * const get_instance_proc_addr_fn = (functions != nullptr && functions->vkGetInstanceProcAddr != nullptr)
                                               ? functions->vkGetInstanceProcAddr
                                               : vkGetInstanceProcAddr;

#define GET_INSTANCE_PROC_ADDR(FuncName)                                                                             \
    if (this->_##FuncName == nullptr)                                                                                \
    {                                                                                                                \
        this->_##FuncName = reinterpret_cast<PFN_##FuncName>(get_instance_proc_addr_fn(this->instance_, #FuncName)); \
    }

    GET_INSTANCE_PROC_ADDR(vkCreateDebugReportCallbackEXT);
    GET_INSTANCE_PROC_ADDR(vkDebugReportMessageEXT);
    GET_INSTANCE_PROC_ADDR(vkDestroyDebugReportCallbackEXT);

#undef GET_INSTANCE_PROC_ADDR
}

// impl_ instance is not created at DebugReportCallbackResource constructor.
vkwp::DebugReportCallbackResource::DebugReportCallbackResource() = default;

// ============================================================================================= //
// Destructor
// ============================================================================================= //
vkwp::DebugReportCallbackResource::Impl_::~Impl_() noexcept
{
    if (this->instance_ != nullptr        //
        && this->_debugReport != nullptr  //
        && this->_vkDestroyDebugReportCallbackEXT != nullptr)
    {
        this->_vkDestroyDebugReportCallbackEXT(this->instance_, this->_debugReport, nullptr);
        this->_debugReport = nullptr;
    }
}

vkwp::DebugReportCallbackResource::~DebugReportCallbackResource() noexcept = default;

// ============================================================================================= //
// initialize()
// ============================================================================================= //
void
vkwp::DebugReportCallbackResource::Impl_::initialize()
{
    VKWP_VALIDATE_ARG(this->_vkCreateDebugReportCallbackEXT);
    VKWP_VALIDATE_ARG(this->_vkDestroyDebugReportCallbackEXT);

    const VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info_ext{
        .sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        .pNext       = nullptr,
        .flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
        .pfnCallback = &DebugReportCallbackResource::Impl_::_debugReportCallback,
        .pUserData   = reinterpret_cast<void *>(this),
    };
    VulkanError::check(  //
        this->_vkCreateDebugReportCallbackEXT(this->instance_,
                                              &debug_report_callback_create_info_ext,
                                              nullptr,
                                              &this->_debugReport),
        "Failed to vkCreateDebugReportCallbackEXT()");
    SPDLOG_DEBUG("Debug callback is setup.");
}

void
vkwp::DebugReportCallbackResource::initialize(const VkInstance                     instance,
                                              std::shared_ptr<DebugReportCallback> callback,
                                              const VulkanFunctions *              functions)
{
    auto impl = std::make_unique<Impl_>(instance, std::move(callback), functions);
    impl->initialize();  // throwable なので impl local variable に一度受ける
    this->impl_ = std::move(impl);
}

// ============================================================================================= //
// [IMPL/PRIVATE] _debugReportCallback
// ============================================================================================= //
VkBool32
vkwp::DebugReportCallbackResource::Impl_::_debugReportCallback(VkDebugReportFlagsEXT      flags,
                                                               VkDebugReportObjectTypeEXT object_type,
                                                               std::uint64_t              object,
                                                               std::size_t                location,
                                                               std::int32_t               message_code,
                                                               const char *               layer_prefix,
                                                               const char *               message,
                                                               void *                     user_data)
{
    auto * const self = reinterpret_cast<DebugReportCallbackResource::Impl_ *>(user_data);
    VKWP_ASSERTION(self != nullptr);
    if (self != nullptr && self->callback_)
    {
        (*self->callback_)(flags, object_type, object, location, message_code, layer_prefix, message);
    }

    VkBool32 ret{ VK_FALSE };
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT || flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        ret = VK_TRUE;
    }
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        SPDLOG_ERROR("Vulkan Validation Error: {}", message);
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        SPDLOG_WARN("Vulkan Validation Warning: {}", message);
    }
    else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        SPDLOG_INFO("Vulkan Validation Info: {}", message);
    }
    else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        SPDLOG_DEBUG("Vulkan Validation Debug: {}", message);
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

vkwp::DebugReportCallback::DebugReportCallback() = default;

vkwp::DebugReportCallback::~DebugReportCallback() noexcept = default;
