#include "vkwp/device.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
// #include "vkwp/buffer.hpp"
// #include "vkwp/command_pool.hpp"
// #include "vkwp/descriptor_pool.hpp"
// #include "vkwp/descriptor_set_layout.hpp"
#include "vkwp/debug_utils_resource.hpp"
// #include "vkwp/device_memory.hpp"
// #include "vkwp/fence.hpp"
// #include "vkwp/framebuffer.hpp"
// #include "vkwp/image.hpp"
// #include "vkwp/image_view.hpp"
#include "vkwp/mutex.hpp"
// #include "vkwp/physical_device.hpp"
// #include "vkwp/pipeline.hpp"
// #include "vkwp/pipeline_layout.hpp"
#include "vkwp/queue.hpp"
// #include "vkwp/render_pass.hpp"
// #include "vkwp/sampler.hpp"
// #include "vkwp/semaphore.hpp"
// #include "vkwp/shader_module.hpp"
// #include "vkwp/surface_khr.hpp"
// #include "vkwp/swapchain_khr.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkDevice>(VkDevice i, const std::tuple<> &) noexcept
{
    vkDestroyDevice(i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Device class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::Device
vkwp::Device::create(const std::string &                          label,
                     const VkPhysicalDevice                       physical_device,
                     const std::vector<VkDeviceQueueCreateInfo> & queue_create_infos,
                     const std::vector<std::string> &             _enabled_layer_names,
                     const std::vector<std::string> &             _enabled_extension_names,
                     const VkPhysicalDeviceFeatures * const       enabled_features,
                     const OptionalCreateArgs &                   opt_args)
{
    VKWP_VALIDATE_ARG(physical_device);
    VKWP_ASSERTION(queue_create_infos.size() <= (std::numeric_limits<std::uint32_t>::max)());
    VKWP_ASSERTION(_enabled_layer_names.size() <= (std::numeric_limits<std::uint32_t>::max)());
    VKWP_ASSERTION(_enabled_extension_names.size() <= (std::numeric_limits<std::uint32_t>::max)());
    Device ret{ label };

    std::vector<const char *> enabled_layer_names;
    enabled_layer_names.reserve(_enabled_layer_names.size());
    for (const auto & name : _enabled_layer_names)
    {
        enabled_layer_names.emplace_back(name.c_str());
    }

    std::vector<const char *> enabled_extension_names;
    enabled_extension_names.reserve(_enabled_extension_names.size());
    for (const auto & name : _enabled_extension_names)
    {
        enabled_extension_names.emplace_back(name.c_str());
    }

    const VkDeviceCreateInfo ci{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = opt_args.p_next,
        .flags                   = 0,
        .queueCreateInfoCount    = static_cast<std::uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos       = queue_create_infos.data(),
        .enabledLayerCount       = static_cast<std::uint32_t>(enabled_layer_names.size()),
        .ppEnabledLayerNames     = enabled_layer_names.data(),
        .enabledExtensionCount   = static_cast<std::uint32_t>(enabled_extension_names.size()),
        .ppEnabledExtensionNames = enabled_extension_names.data(),
        .pEnabledFeatures        = enabled_features,
    };
    VulkanError::check(  //
        vkCreateDevice(physical_device, &ci, nullptr, ret._reset()),
        "Failed to vkCreateDevice().");
    return ret;
}

void
vkwp::Device::waitIdle() const
{
    VKWP_VALIDATE_THIS;
    VulkanError::check(vkDeviceWaitIdle(*this), "Failed to vkDeviceWaitIdle().");
}

vkwp::Queue
vkwp::Device::getDeviceQueue(const std::uint32_t        queue_family_index,
                             const std::uint32_t        queue_index,
                             const DeviceQueueGetArgs & opt_args) const
{
    VKWP_VALIDATE_THIS;
    return this->getDeviceQueue(queue_family_index, queue_index, {}, opt_args);
}

vkwp::Queue
vkwp::Device::getDeviceQueue(const std::uint32_t        queue_family_index,
                             const std::uint32_t        queue_index,
                             std::unique_ptr<Mutex> &&  mutex,
                             const DeviceQueueGetArgs & opt_args) const
{
    VKWP_VALIDATE_THIS;
    Queue ret{ this->getLabel() + "/Queue"                        //
               + "/Family[" + std::to_string(queue_family_index)  //
               + "]/Index[" + std::to_string(queue_index) + "]" };
    vkGetDeviceQueue(*this, queue_family_index, queue_index, ret._reset());
    ret.queue_family_index_ = queue_family_index;
    ret.mutex_              = std::move(mutex);
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(*this,
                                                         VK_OBJECT_TYPE_QUEUE,
                                                         static_cast<VkQueue>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

#if 0

#    if VKWP_SUPPORT_WIN32_SURFACE && defined(_WIN32) && _WIN32
vkwp::DeviceMemory
vkwp::Device::importWin32ExternalMemory(const std::string &                      label,
                                        const PhysicalDevice &                   physical_device,
                                        const std::size_t                        size,
                                        const VkExternalMemoryHandleTypeFlagBits handle_type,
                                        void * const                             handle,
                                        const wchar_t * const                    name,
                                        const VkImage &                          image,
                                        const VkBuffer &                         buffer) const
{
    // 互換性のため
    return DeviceMemory::importWin32ExternalMemory(label,
                                                   *this,
                                                   physical_device,
                                                   size,
                                                   handle_type,
                                                   handle,
                                                   name,
                                                   image,
                                                   buffer);
}
#    endif

vkwp::CommandPool
vkwp::Device::createCommandPool(const std::string &            label,
                                const std::uint32_t            queue_family_index,
                                std::unique_ptr<Mutex> &&      mutex,
                                const VkCommandPoolCreateFlags flags,
                                const void * const             p_next) const
{
}



vkwp::Semaphore
vkwp::Device::createSemaphore(const std::string & label) const
{
    Semaphore                   ret{ label };
    const VkSemaphoreCreateInfo ci{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    VulkanError::check(  //
        vkCreateSemaphore(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateSemaphore().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

vkwp::SwapchainKHR
vkwp::Device::createSwapchainKHR(const std::string &                 label,
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
                                 const VkSwapchainCreateFlagsKHR     flags,
                                 const void * const                  p_next) const
{
    SwapchainKHR                   ret{ label };
    const VkSwapchainCreateInfoKHR ci{
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = p_next,
        .flags                 = flags,
        .surface               = surface,
        .minImageCount         = min_image_count,
        .imageFormat           = image_format,
        .imageColorSpace       = image_color_space,
        .imageExtent           = image_extent,
        .imageArrayLayers      = image_array_layers,
        .imageUsage            = image_usage,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = pre_transform,
        .compositeAlpha        = composite_alpha,
        .presentMode           = present_mode,
        .clipped               = clipped,
        .oldSwapchain          = old_swapchain,
    };
    VulkanError::check(  //
        vkCreateSwapchainKHR(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateSwapchainKHR().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

vkwp::RenderPass
vkwp::Device::createRenderPass(const std::string &                          label,
                               const std::vector<VkAttachmentDescription> & attachments,
                               const std::vector<VkSubpassDescription> &    subpasses,
                               const std::vector<VkSubpassDependency> &     dependencies,
                               const VkRenderPassCreateFlags                flags,
                               const void * const                           p_next) const
{
}

vkwp::Framebuffer
vkwp::Device::createFramebuffer(const std::string &            label,
                                const VkRenderPass             render_pass,
                                const std::vector<VkImageView> attachments,
                                const std::uint32_t            width,
                                const std::uint32_t            height,
                                const std::uint32_t            layers,
                                const VkFramebufferCreateFlags flags,
                                const void * const             p_next) const
{
}

vkwp::Sampler
vkwp::Device::createSampler(const std::string &        label,
                            const VkFilter             mag_filter,
                            const VkFilter             min_filter,
                            const VkSamplerMipmapMode  mipmap_mode,
                            const VkSamplerAddressMode address_mode_u,
                            const VkSamplerAddressMode address_mode_v,
                            const VkSamplerAddressMode address_mode_w,
                            const VkBool32             anisotropy_enable,
                            const float                max_anisotropy,
                            const VkBorderColor        border_color,
                            const float                mip_lod_bias,
                            const VkBool32             compare_enable,
                            const VkCompareOp          compare_op,
                            const float                min_lod,
                            const float                max_lod,
                            const VkBool32             unnormalized_coordinates,
                            const VkSamplerCreateFlags flags,
                            const void * const         p_next) const
{
    Sampler                   ret{ label };
    const VkSamplerCreateInfo ci{
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = p_next,
        .flags                   = flags,
        .magFilter               = mag_filter,
        .minFilter               = min_filter,
        .mipmapMode              = mipmap_mode,
        .addressModeU            = address_mode_u,
        .addressModeV            = address_mode_v,
        .addressModeW            = address_mode_w,
        .mipLodBias              = mip_lod_bias,
        .anisotropyEnable        = anisotropy_enable,
        .maxAnisotropy           = max_anisotropy,
        .compareEnable           = compare_enable,
        .compareOp               = compare_op,
        .minLod                  = min_lod,
        .maxLod                  = max_lod,
        .borderColor             = border_color,
        .unnormalizedCoordinates = unnormalized_coordinates,
    };
    VulkanError::check(  //
        vkCreateSampler(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateSampler().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

vkwp::DescriptorPool
vkwp::Device::createDescriptorPool(const std::string &                       label,
                                   const std::uint32_t                       max_sets,
                                   const std::vector<VkDescriptorPoolSize> & pool_sizes,
                                   const VkDescriptorPoolCreateFlags         flags,
                                   const void * const                        p_next) const
{
    DescriptorPool                   ret{ label };
    const VkDescriptorPoolCreateInfo ci{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = p_next,
        .flags         = flags,
        .maxSets       = max_sets,
        .poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data(),
    };
    VulkanError::check(  //
        vkCreateDescriptorPool(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateDescriptorPool().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    ret.setDebugUtilsResource(this->debug_utils_resource_);
    return ret;
}

vkwp::DescriptorSetLayout
vkwp::Device::createDescriptorSetLayout(const std::string &                               label,
                                        const std::vector<VkDescriptorSetLayoutBinding> & bindings,
                                        const VkDescriptorSetLayoutCreateFlags            flags,
                                        const void * const                                p_next) const
{
    DescriptorSetLayout                   ret{ label };
    const VkDescriptorSetLayoutCreateInfo ci{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = p_next,
        .flags        = flags,
        .bindingCount = static_cast<std::uint32_t>(bindings.size()),
        .pBindings    = bindings.data(),
    };
    VulkanError::check(  //
        vkCreateDescriptorSetLayout(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateDescriptorSetLayout().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

vkwp::ShaderModule
vkwp::Device::createShaderModule(const std::string & label, const std::vector<std::uint32_t> & code) const
{
    ShaderModule                   ret{ label };
    const VkShaderModuleCreateInfo ci{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0,
        .codeSize = code.size() * sizeof(std::remove_reference_t<decltype(code)>::value_type),
        .pCode    = code.data(),
    };
    VulkanError::check(  //
        vkCreateShaderModule(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateShaderModule().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

vkwp::ShaderModule
vkwp::Device::createShaderModule(const std::string &         label,
                                 const std::uint32_t * const data,
                                 const std::size_t           data_length) const
{
    ShaderModule                   ret{ label };
    const VkShaderModuleCreateInfo ci{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0,
        .codeSize = data_length * sizeof(std::uint32_t),
        .pCode    = data,
    };
    VulkanError::check(  //
        vkCreateShaderModule(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateShaderModule().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

vkwp::PipelineLayout
vkwp::Device::createPipelineLayout(const std::string &                        label,
                                   const std::vector<VkDescriptorSetLayout> & set_layouts,
                                   const std::vector<VkPushConstantRange> &   push_constant_ranges,
                                   const VkPipelineLayoutCreateFlags          flags,
                                   const void * const                         p_next) const
{
}

vkwp::Pipeline
vkwp::Device::createGraphicsPipeline(const std::string &                                  label,
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
                                     const VkPipeline                                     base_pipeline_handle,
                                     const int32_t                                        base_pipeline_index,
                                     const VkPipelineCreateFlags                          flags,
                                     const void * const                                   p_next) const
{
    Pipeline                           ret{ label };
    const VkGraphicsPipelineCreateInfo ci{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = p_next,
        .flags               = flags,
        .stageCount          = static_cast<std::uint32_t>(stages.size()),
        .pStages             = stages.data(),
        .pVertexInputState   = &vertex_input_state,
        .pInputAssemblyState = &input_assembly_state,
        .pTessellationState  = &tessellation_state,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterization_state,
        .pMultisampleState   = &multisample_state,
        .pDepthStencilState  = &depth_stencil_state,
        .pColorBlendState    = &color_blend_state,
        .pDynamicState       = &dynamic_state,
        .layout              = layout,
        .renderPass          = render_pass,
        .subpass             = subpass,
        .basePipelineHandle  = base_pipeline_handle,
        .basePipelineIndex   = base_pipeline_index,
    };
    VulkanError::check(  //
        vkCreateGraphicsPipelines(this->_ref(), nullptr, 1, &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateGraphicsPipelines().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}

vkwp::Pipeline
vkwp::Device::createComputePipeline(const std::string &         label,
                                    const VkPipelineLayout      layout,
                                    const VkShaderModule        shader_module,
                                    const VkPipeline            base_pipeline_handle,
                                    const int32_t               base_pipeline_index,
                                    const VkPipelineCreateFlags flags,
                                    const void * const          p_next) const
{
    Pipeline                          ret{ label };
    const VkComputePipelineCreateInfo ci{
        .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext  = p_next,
        .flags  = flags,
        .stage  = VkPipelineShaderStageCreateInfo{ .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                   .pNext  = nullptr,
                                                   .flags  = 0,
                                                   .stage  = VK_SHADER_STAGE_COMPUTE_BIT,
                                                   .module = shader_module,
                                                   .pName  = "main",
                                                   .pSpecializationInfo = nullptr },
        .layout = layout,
        .basePipelineHandle = base_pipeline_handle,
        .basePipelineIndex  = base_pipeline_index,
    };
    VulkanError::check(  //
        vkCreateComputePipelines(this->_ref(), nullptr, 1, &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateGraphicsPipelines().");
    this->debug_utils_resource_.registDebugUtilsObjectName(ret);
    return ret;
}
#endif