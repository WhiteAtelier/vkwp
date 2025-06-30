#include "vkwp/debug_utils_resource.hpp"

#include "impl/assert.hpp"
#include "vkwp/buffer.hpp"
#include "vkwp/command_buffer.hpp"
#include "vkwp/command_pool.hpp"
#include "vkwp/descriptor_pool.hpp"
#include "vkwp/descriptor_set.hpp"
#include "vkwp/descriptor_set_layout.hpp"
#include "vkwp/device.hpp"
#include "vkwp/device_memory.hpp"
#include "vkwp/fence.hpp"
#include "vkwp/framebuffer.hpp"
#include "vkwp/image.hpp"
#include "vkwp/image_view.hpp"
#include "vkwp/pipeline.hpp"
#include "vkwp/pipeline_layout.hpp"
#include "vkwp/queue.hpp"
#include "vkwp/render_pass.hpp"
#include "vkwp/sampler.hpp"
#include "vkwp/semaphore.hpp"
#include "vkwp/shader_module.hpp"
#include "vkwp/swapchain_khr.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

#include <bit>

namespace vkwp {

class DebugUtilsResource::Impl_ final
{
public:
    Impl_(const VkInstance & instance);
    ~Impl_() noexcept;

    void
    initialize();

    void
    registDebugUtilsObjectName(const VkDevice      device,
                               const VkObjectType  object_type,
                               const void * const  ptr,
                               const std::string & name) const;

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
vkwp::DebugUtilsResource::Impl_::registDebugUtilsObjectName(const VkDevice      device,
                                                            const VkObjectType  object_type,
                                                            const void * const  ptr,
                                                            const std::string & name) const
{
    const VkDebugUtilsObjectNameInfoEXT object_name_info_ext{
        .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext        = nullptr,
        .objectType   = object_type,
        .objectHandle = std::bit_cast<std::uint64_t>(ptr),
        .pObjectName  = name.c_str(),
    };
    VulkanError::check(  //
        this->pfn_vkSetDebugUtilsObjectNameEXT_(device, &object_name_info_ext),
        "Failed to (pfn)vkSetDebugUtilsObjectNameEXT()");
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const VkDevice &    device,
                                                     const VkObjectType  object_type,
                                                     const void * const  ptr,
                                                     const std::string & name) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(device, VK_OBJECT_TYPE_BUFFER, ptr, name);
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::CommandBuffer & command_buffer) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(command_buffer._getParent(),
                                                VK_OBJECT_TYPE_COMMAND_BUFFER,
                                                command_buffer,
                                                command_buffer.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::CommandPool & command_pool) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(command_pool._getParent(),
                                                VK_OBJECT_TYPE_COMMAND_POOL,
                                                command_pool,
                                                command_pool.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::DescriptorPool & descriptor_pool) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(descriptor_pool._getParent(),
                                                VK_OBJECT_TYPE_DESCRIPTOR_POOL,
                                                descriptor_pool,
                                                descriptor_pool.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::DescriptorSet & descriptor_set) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(descriptor_set._getParent(),
                                                VK_OBJECT_TYPE_DESCRIPTOR_SET,
                                                descriptor_set,
                                                descriptor_set.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::DescriptorSetLayout & descriptor_set_layout) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(descriptor_set_layout._getParent(),
                                                VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
                                                descriptor_set_layout,
                                                descriptor_set_layout.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::DeviceMemory & memory) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(memory._getParent(),
                                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                                memory,
                                                memory.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::Fence & fence) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(fence._getParent(),  //
                                                VK_OBJECT_TYPE_FENCE,
                                                fence,
                                                fence.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::Framebuffer & framebuffer) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(framebuffer._getParent(),  //
                                                VK_OBJECT_TYPE_FRAMEBUFFER,
                                                framebuffer,
                                                framebuffer.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::Image & image) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(image._getParent(),  //
                                                VK_OBJECT_TYPE_IMAGE,
                                                image,
                                                image.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::ImageView & image_view) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(image_view._getParent(),  //
                                                VK_OBJECT_TYPE_IMAGE_VIEW,
                                                image_view,
                                                image_view.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::Pipeline & pipeline) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(pipeline._getParent(),  //
                                                VK_OBJECT_TYPE_PIPELINE,
                                                pipeline,
                                                pipeline.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::PipelineLayout & pipeline_layout) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(pipeline_layout._getParent(),  //
                                                VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                                                pipeline_layout,
                                                pipeline_layout.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::Queue & queue) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(queue._getParent(),  //
                                                VK_OBJECT_TYPE_QUEUE,
                                                queue,
                                                queue.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::RenderPass & render_pass) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(render_pass._getParent(),  //
                                                VK_OBJECT_TYPE_RENDER_PASS,
                                                render_pass,
                                                render_pass.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::Sampler & sampler) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(sampler._getParent(),  //
                                                VK_OBJECT_TYPE_SAMPLER,
                                                sampler,
                                                sampler.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::Semaphore & semaphore) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(semaphore._getParent(),  //
                                                VK_OBJECT_TYPE_SEMAPHORE,
                                                semaphore,
                                                semaphore.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::ShaderModule & shader_module) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(shader_module._getParent(),  //
                                                VK_OBJECT_TYPE_SHADER_MODULE,
                                                shader_module,
                                                shader_module.getLabel());
    }
}

void
vkwp::DebugUtilsResource::registDebugUtilsObjectName(const vkwp::SwapchainKHR & swapchain_khr) const
{
    if (this->impl_)
    {
        this->impl_->registDebugUtilsObjectName(swapchain_khr._getParent(),  //
                                                VK_OBJECT_TYPE_SWAPCHAIN_KHR,
                                                swapchain_khr,
                                                swapchain_khr.getLabel());
    }
}
