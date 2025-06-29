
#ifndef VKWP_DEVICE_HPP
#define VKWP_DEVICE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"
//
#include "debug_utils_resource.hpp"

#include <memory>
#include <vector>

namespace vkwp {

///
/// @brief VkDevice wrapper.
///
class Device final : public Resource<VkDevice, void, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit Device();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit Device(const std::string & name);

    void
    setDebugUtilsResource(const DebugUtilsResource & debug_utils_resource);

    void
    waitIdle() const;

    [[nodiscard]]
    Queue
    getDeviceQueue(const std::uint32_t       queue_family_index,
                   const std::uint32_t       queue_index,
                   std::unique_ptr<Mutex> && mutex = nullptr) const;

    [[nodiscard]]
    Buffer
    createBuffer(const std::string &       label,
                 const VkDeviceSize        size,
                 const VkBufferUsageFlags  usage,
                 const VkBufferCreateFlags flags  = 0,
                 const void * const        p_next = nullptr) const;

    [[nodiscard]]
    Image
    createImage(const std::string &         label,
                const VkImageType           image_type,
                const VkFormat              format,
                const VkExtent3D            extent,
                const std::uint32_t         mip_levels,
                const std::uint32_t         array_layers,
                const VkSampleCountFlagBits samples,
                const VkImageTiling         tiling,
                const VkImageUsageFlags     usage,
                const VkImageCreateFlags    flags  = 0,
                const void * const          p_next = nullptr) const;

    [[nodiscard]]
    ImageView
    createImageView(const std::string &             label,
                    const VkImage                   image,
                    const VkImageViewType           view_type,
                    const VkFormat                  format,
                    const VkComponentMapping &      components,
                    const VkImageSubresourceRange & subresource_range,
                    const VkImageViewCreateFlags    flags  = 0,
                    const void * const              p_next = nullptr) const;

    [[nodiscard]]
    DeviceMemory
    allocateDeviceMemory(const std::string &          label,
                         const PhysicalDevice &       physical_device,
                         const VkMemoryRequirements & memory_requirements,
                         const VkMemoryPropertyFlags  memory_property_flags,
                         const void * const           p_next = nullptr) const;

#if defined(WIN32) && WIN32
    [[nodiscard]]
    DeviceMemory
    importWin32ExternalMemory(const std::string &                      label,
                              const PhysicalDevice &                   physical_device,
                              const std::size_t                        size,
                              const VkExternalMemoryHandleTypeFlagBits handle_type,
                              void * const                             handle,
                              const wchar_t * const                    name,
                              const VkImage &                          image,
                              const VkBuffer &                         buffer) const;
#endif

    [[nodiscard]]
    CommandPool
    createCommandPool(const std::string &            label,
                      const std::uint32_t            queue_family_index,
                      std::unique_ptr<Mutex> &&      mutex  = nullptr,
                      const VkCommandPoolCreateFlags flags  = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                      const void * const             p_next = nullptr) const;

    [[nodiscard]]
    Fence
    createFence(const std::string &      label,
                const VkFenceCreateFlags flags  = 0,
                const void * const       p_next = nullptr) const;

    [[nodiscard]]
    Semaphore
    createSemaphore(const std::string & label) const;

    [[nodiscard]]
    SwapchainKHR
    createSwapchainKHR(const std::string &                 label,
                       const VkSurfaceKHR                  surface,
                       const std::uint32_t                 min_image_count,
                       const VkFormat                      image_format,
                       const VkColorSpaceKHR               image_color_space,
                       const VkExtent2D                    image_extent,
                       const std::uint32_t                 image_array_layers,
                       const VkImageUsageFlags             image_usage,
                       const VkSurfaceTransformFlagBitsKHR pre_transform,
                       const VkCompositeAlphaFlagBitsKHR   composite_alpha,
                       const VkPresentModeKHR              present_mode,
                       const VkBool32                      clipped,
                       const VkSwapchainKHR                old_swapchain,
                       const VkSwapchainCreateFlagsKHR     flags  = 0,
                       const void * const                  p_next = nullptr) const;

    [[nodiscard]]
    RenderPass
    createRenderPass(const std::string &                          label,
                     const std::vector<VkAttachmentDescription> & attachments,
                     const std::vector<VkSubpassDescription> &    subpasses,
                     const std::vector<VkSubpassDependency> &     dependencies,
                     const VkRenderPassCreateFlags                flags  = 0,
                     const void * const                           p_next = nullptr) const;

    [[nodiscard]]
    Framebuffer
    createFramebuffer(const std::string &            label,
                      const VkRenderPass             render_pass,
                      const std::vector<VkImageView> attachments,
                      const std::uint32_t            width,
                      const std::uint32_t            height,
                      const std::uint32_t            layers,
                      const VkFramebufferCreateFlags flags  = 0,
                      const void * const             p_next = nullptr) const;

    [[nodiscard]]
    Sampler
    createSampler(const std::string &        label,
                  const VkFilter             mag_filter,
                  const VkFilter             min_filter,
                  const VkSamplerMipmapMode  mipmap_mode,
                  const VkSamplerAddressMode address_mode_u,
                  const VkSamplerAddressMode address_mode_v,
                  const VkSamplerAddressMode address_mode_w,
                  const VkBool32             anisotropy_enable,
                  const float                max_anisotropy,
                  const VkBorderColor        border_color,
                  const float                mip_lod_bias             = 1.0f,
                  const VkBool32             compare_enable           = VK_FALSE,
                  const VkCompareOp          compare_op               = VK_COMPARE_OP_ALWAYS,
                  const float                min_lod                  = 0.0f,
                  const float                max_lod                  = VK_LOD_CLAMP_NONE,
                  const VkBool32             unnormalized_coordinates = VK_FALSE,
                  const VkSamplerCreateFlags flags                    = 0,
                  const void * const         p_next                   = nullptr) const;

    [[nodiscard]]
    DescriptorPool
    createDescriptorPool(const std::string &                       label,
                         const std::uint32_t                       max_sets,
                         const std::vector<VkDescriptorPoolSize> & pool_sizes,
                         const VkDescriptorPoolCreateFlags flags  = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                         const void * const                p_next = nullptr) const;

    [[nodiscard]]
    DescriptorSetLayout
    createDescriptorSetLayout(const std::string &                               label,
                              const std::vector<VkDescriptorSetLayoutBinding> & bindings,
                              const VkDescriptorSetLayoutCreateFlags            flags  = 0,
                              const void * const                                p_next = nullptr) const;

    [[nodiscard]]
    ShaderModule
    createShaderModule(const std::string & label, const std::vector<std::uint32_t> & code) const;

    [[nodiscard]]
    ShaderModule
    createShaderModule(const std::string &         label,
                       const std::uint32_t * const data,
                       const std::size_t           data_length) const;

    [[nodiscard]]
    PipelineLayout
    createPipelineLayout(const std::string &                        label,
                         const std::vector<VkDescriptorSetLayout> & set_layouts,
                         const std::vector<VkPushConstantRange> &   push_constant_ranges,
                         const VkPipelineLayoutCreateFlags          flags  = 0,
                         const void * const                         p_next = nullptr) const;

    [[nodiscard]]
    Pipeline
    createGraphicsPipeline(const std::string &                                  label,
                           const std::vector<VkPipelineShaderStageCreateInfo> & stages,
                           const VkPipelineVertexInputStateCreateInfo &         vertex_input_state,
                           const VkPipelineInputAssemblyStateCreateInfo &       input_assembly_state,
                           const VkPipelineTessellationStateCreateInfo &        tessellation_state,
                           const VkPipelineViewportStateCreateInfo &            viewport_state,
                           const VkPipelineRasterizationStateCreateInfo &       rasterization_state,
                           const VkPipelineMultisampleStateCreateInfo &         multisample_state,
                           const VkPipelineDepthStencilStateCreateInfo &        depth_stencil_state,
                           const VkPipelineColorBlendStateCreateInfo &          color_blend_state,
                           const VkPipelineDynamicStateCreateInfo &             dynamic_state,
                           const VkPipelineLayout                               layout,
                           const VkRenderPass                                   render_pass,
                           const std::uint32_t                                  subpass,
                           const VkPipeline                                     base_pipeline_handle = nullptr,
                           const int32_t                                        base_pipeline_index  = 0,
                           const VkPipelineCreateFlags                          flags                = 0,
                           const void * const                                   p_next               = nullptr) const;

    [[nodiscard]]
    Pipeline
    createComputePipeline(const std::string &         label,
                          const VkPipelineLayout      layout,
                          const VkShaderModule        shader_module,
                          const VkPipeline            base_pipeline_handle = nullptr,
                          const int32_t               base_pipeline_index  = 0,
                          const VkPipelineCreateFlags flags                = 0,
                          const void * const          p_next               = nullptr) const;

private:
    DebugUtilsResource debug_utils_resource_;
};

}  // namespace vkwp

#endif
