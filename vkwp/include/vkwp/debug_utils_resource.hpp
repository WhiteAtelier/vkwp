#ifndef VKWP_DEBUG_UTILS_RESOURCE_HPP
#define VKWP_DEBUG_UTILS_RESOURCE_HPP

#include "fwd.hpp"
#include "vulkan.hpp"

#include <memory>

namespace vkwp {

///
/// @brief Vulkan Debug Utils.
///
class DebugUtilsResource
{
public:
    DebugUtilsResource();
    DebugUtilsResource(const DebugUtilsResource &);
    DebugUtilsResource(DebugUtilsResource &&) noexcept;
    ~DebugUtilsResource() noexcept;

    DebugUtilsResource &
    operator=(const DebugUtilsResource &) &;

    DebugUtilsResource &
    operator=(DebugUtilsResource &&) & noexcept;

    ///
    /// @brief Instance 生成後に呼び出すべき初期化関数.
    ///
    /// Debug utils を使用しない場合は呼び出さない.
    ///
    /// @param instance
    ///     初期化済み Vulkan Instance.
    ///
    void
    initialize(const VkInstance instance);

    void
    registDebugUtilsObjectName(const vkwp::Buffer & buffer) const;

    void
    registDebugUtilsObjectName(const vkwp::CommandBuffer & command_buffer) const;

    void
    registDebugUtilsObjectName(const vkwp::CommandPool & command_pool) const;

    void
    registDebugUtilsObjectName(const vkwp::DescriptorPool & descriptor_pool) const;

    void
    registDebugUtilsObjectName(const vkwp::DescriptorSet & descriptor_set) const;

    void
    registDebugUtilsObjectName(const vkwp::DescriptorSetLayout & descriptor_set_layout) const;

    void
    registDebugUtilsObjectName(const vkwp::Device & device) const;

    void
    registDebugUtilsObjectName(const vkwp::DeviceMemory & memory) const;

    void
    registDebugUtilsObjectName(const vkwp::Fence & fence) const;

    void
    registDebugUtilsObjectName(const vkwp::Framebuffer & framebuffer) const;

    void
    registDebugUtilsObjectName(const vkwp::Image & image) const;

    void
    registDebugUtilsObjectName(const vkwp::ImageView & image_view) const;

    void
    registDebugUtilsObjectName(const vkwp::Pipeline & pipeline) const;

    void
    registDebugUtilsObjectName(const vkwp::PipelineLayout & pipeline_layout) const;

    void
    registDebugUtilsObjectName(const vkwp::Queue & queue) const;

    void
    registDebugUtilsObjectName(const vkwp::RenderPass & render_pass) const;

    void
    registDebugUtilsObjectName(const vkwp::Sampler & sampler) const;

    void
    registDebugUtilsObjectName(const vkwp::Semaphore & semaphore) const;

    void
    registDebugUtilsObjectName(const vkwp::ShaderModule & shader_module) const;

    void
    registDebugUtilsObjectName(const vkwp::SwapchainKHR & swapchain_khr) const;

private:
    class Impl_;
    std::shared_ptr<Impl_> impl_;
};

}  // namespace vkwp

#endif
