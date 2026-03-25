#include "vkwp/debug_utils_resource.hpp"

#include "impl/assert.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

#include <bit>
#include <string>
#include <string_view>

namespace vkwp {

class DebugUtilsResource::Impl_ final
{
public:
    Impl_(const VkInstance & instance);
    ~Impl_() noexcept;

    void
    initialize();

    void
    registDebugUtilsObjectName(const VkDevice         device,
                               const VkObjectType     object_type,
                               const void * const     ptr,
                               const std::string_view name) const;

private:
    const VkInstance                   instance_                           = nullptr;
    PFN_vkSetDebugUtilsObjectNameEXT   pfn_vkSetDebugUtilsObjectNameEXT_   = nullptr;
    PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT_ = nullptr;
    PFN_vkQueueEndDebugUtilsLabelEXT   pfn_vkQueueEndDebugUtilsLabelEXT_   = nullptr;
    PFN_vkCmdBeginDebugUtilsLabelEXT   pfn_vkCmdBeginDebugUtilsLabelEXT_   = nullptr;
    PFN_vkCmdEndDebugUtilsLabelEXT     pfn_vkCmdEndDebugUtilsLabelEXT_     = nullptr;
    PFN_vkCmdInsertDebugUtilsLabelEXT  pfn_vkCmdInsertDebugUtilsLabelEXT_  = nullptr;
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

vkwp::DebugUtilsResource::Impl_::Impl_(const VkInstance & instance)
    : instance_{ instance }
    , GET_INSTANCE_PROC_ADDR(vkSetDebugUtilsObjectNameEXT)
    , GET_INSTANCE_PROC_ADDR(vkQueueBeginDebugUtilsLabelEXT)
    , GET_INSTANCE_PROC_ADDR(vkQueueEndDebugUtilsLabelEXT)
    , GET_INSTANCE_PROC_ADDR(vkCmdBeginDebugUtilsLabelEXT)
    , GET_INSTANCE_PROC_ADDR(vkCmdEndDebugUtilsLabelEXT)
    , GET_INSTANCE_PROC_ADDR(vkCmdInsertDebugUtilsLabelEXT)
{}
#undef GET_INSTANCE_PROC_ADDR

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
vkwp::DebugUtilsResource::initialize(const VkInstance instance)
{
    this->impl_ = std::make_shared<Impl_>(instance);
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
    std::string                         name_str{ name };  // guarantee null terminated
    const VkDebugUtilsObjectNameInfoEXT object_name_info_ext{
        .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext        = nullptr,
        .objectType   = object_type,
        .objectHandle = std::bit_cast<std::uint64_t>(ptr),
        .pObjectName  = name_str.c_str(),
    };
    VulkanError::check(  //
        this->pfn_vkSetDebugUtilsObjectNameEXT_(device, &object_name_info_ext),
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
