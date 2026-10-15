// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "vkwp/debug_utils_resource.hpp"

#include "impl/assert.hpp"

#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

#include <spdlog/spdlog.h>

#include <bit>
#include <string>
#include <string_view>

namespace vkwp {

class DebugUtilsResource::Impl_ final
{
public:
    Impl_(const VkInstance & instance, const VulkanFunctions * functions);
    ~Impl_() noexcept;

    void
    initialize();

    void
    registDebugUtilsObjectName(const VkDevice         device,
                               const VkObjectType     object_type,
                               const void * const     ptr,
                               const std::string_view name) const;

private:
    const VkInstance                   instance_                       = nullptr;
    PFN_vkSetDebugUtilsObjectNameEXT   _vkSetDebugUtilsObjectNameEXT   = nullptr;
    PFN_vkQueueBeginDebugUtilsLabelEXT _vkQueueBeginDebugUtilsLabelEXT = nullptr;
    PFN_vkQueueEndDebugUtilsLabelEXT   _vkQueueEndDebugUtilsLabelEXT   = nullptr;
    PFN_vkCmdBeginDebugUtilsLabelEXT   _vkCmdBeginDebugUtilsLabelEXT   = nullptr;
    PFN_vkCmdEndDebugUtilsLabelEXT     _vkCmdEndDebugUtilsLabelEXT     = nullptr;
    PFN_vkCmdInsertDebugUtilsLabelEXT  _vkCmdInsertDebugUtilsLabelEXT  = nullptr;
};

}  // namespace vkwp

// ============================================================================================= //
// Constructor
// ============================================================================================= //
vkwp::DebugUtilsResource::Impl_::Impl_(const VkInstance & instance, const VulkanFunctions * functions)
    : instance_{ instance }
{
    if (functions != nullptr)
    {
        this->_vkSetDebugUtilsObjectNameEXT   = functions->vkSetDebugUtilsObjectNameEXT;
        this->_vkQueueBeginDebugUtilsLabelEXT = functions->vkQueueBeginDebugUtilsLabelEXT;
        this->_vkQueueEndDebugUtilsLabelEXT   = functions->vkQueueEndDebugUtilsLabelEXT;
        this->_vkCmdBeginDebugUtilsLabelEXT   = functions->vkCmdBeginDebugUtilsLabelEXT;
        this->_vkCmdEndDebugUtilsLabelEXT     = functions->vkCmdEndDebugUtilsLabelEXT;
        this->_vkCmdInsertDebugUtilsLabelEXT  = functions->vkCmdInsertDebugUtilsLabelEXT;
    }

    auto * const get_instance_proc_addr_fn = (functions != nullptr && functions->vkGetInstanceProcAddr != nullptr)
                                               ? functions->vkGetInstanceProcAddr
                                               : vkGetInstanceProcAddr;

#define GET_INSTANCE_PROC_ADDR(FuncName)                                                                             \
    if (this->_##FuncName == nullptr)                                                                                \
    {                                                                                                                \
        this->_##FuncName = reinterpret_cast<PFN_##FuncName>(get_instance_proc_addr_fn(this->instance_, #FuncName)); \
    }

    GET_INSTANCE_PROC_ADDR(vkSetDebugUtilsObjectNameEXT);
    GET_INSTANCE_PROC_ADDR(vkQueueBeginDebugUtilsLabelEXT);
    GET_INSTANCE_PROC_ADDR(vkQueueEndDebugUtilsLabelEXT);
    GET_INSTANCE_PROC_ADDR(vkCmdBeginDebugUtilsLabelEXT);
    GET_INSTANCE_PROC_ADDR(vkCmdEndDebugUtilsLabelEXT);
    GET_INSTANCE_PROC_ADDR(vkCmdInsertDebugUtilsLabelEXT);

#undef GET_INSTANCE_PROC_ADDR
}

// impl_ instance is not created at DebugUtilsResource constructor.
vkwp::DebugUtilsResource::DebugUtilsResource() = default;

// ============================================================================================= //
// Destructor
// ============================================================================================= //
vkwp::DebugUtilsResource::Impl_::~Impl_() noexcept = default;

vkwp::DebugUtilsResource::~DebugUtilsResource() noexcept = default;

// ============================================================================================= //
// Copy / Move - Constructor / Assigner
// ============================================================================================= //
vkwp::DebugUtilsResource::DebugUtilsResource(const DebugUtilsResource &) = default;

vkwp::DebugUtilsResource::DebugUtilsResource(DebugUtilsResource &&) noexcept = default;

vkwp::DebugUtilsResource &
vkwp::DebugUtilsResource::operator=(const DebugUtilsResource &) & = default;

vkwp::DebugUtilsResource &
vkwp::DebugUtilsResource::operator=(DebugUtilsResource &&) & noexcept
    = default;

// ============================================================================================= //
// initialize()
// ============================================================================================= //
void
vkwp::DebugUtilsResource::initialize(const VkInstance instance, const VulkanFunctions * functions)
{
    this->impl_ = std::make_shared<Impl_>(instance, functions);
}

// ============================================================================================= //
// registDebugUtilsObjectName()
// ============================================================================================= //
void
vkwp::DebugUtilsResource::Impl_::registDebugUtilsObjectName(const VkDevice         device,
                                                            const VkObjectType     object_type,
                                                            const void * const     ptr,
                                                            const std::string_view name) const
{
    VKWP_VALIDATE_ARG(this->_vkSetDebugUtilsObjectNameEXT);

    std::string                         name_str{ name };  // guarantee null terminated
    const VkDebugUtilsObjectNameInfoEXT object_name_info_ext{
        .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext        = nullptr,
        .objectType   = object_type,
        .objectHandle = std::bit_cast<std::uint64_t>(ptr),
        .pObjectName  = name_str.c_str(),
    };
    VulkanError::check(  //
        this->_vkSetDebugUtilsObjectNameEXT(device, &object_name_info_ext),
        "Failed to (pfn)vkSetDebugUtilsObjectNameEXT()");
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const VkDevice &       device,
                                                     const VkObjectType     object_type,
                                                     const void * const     ptr,
                                                     const std::string_view name) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(device, object_type, ptr, name);
    }
}
