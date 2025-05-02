#include "vkwp/device.hpp"

#include "impl/assert.hpp"
#include "vkwp/buffer.hpp"
#include "vkwp/command_pool.hpp"
#include "vkwp/descriptor_pool.hpp"
#include "vkwp/descriptor_set_layout.hpp"
#include "vkwp/device_memory.hpp"
#include "vkwp/fence.hpp"
#include "vkwp/framebuffer.hpp"
#include "vkwp/image.hpp"
#include "vkwp/image_view.hpp"
#include "vkwp/mutex.hpp"
#include "vkwp/physical_device.hpp"
#include "vkwp/pipeline.hpp"
#include "vkwp/pipeline_layout.hpp"
#include "vkwp/queue.hpp"
#include "vkwp/render_pass.hpp"
#include "vkwp/sampler.hpp"
#include "vkwp/semaphore.hpp"
#include "vkwp/shader_module.hpp"
#include "vkwp/surface_khr.hpp"
#include "vkwp/swapchain_khr.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

vkwp::Device::Device() = default;

vkwp::Device::Device(const std::string & name)
    : Resource<VkDevice, void, void>{ name }
{}

void
vkwp::Device::waitIdle() const
{
    VulkanError::check(  //
        vkDeviceWaitIdle(this->_ref()),
        "Failed to vkDeviceWaitIdle().");
}

vkwp::Queue
vkwp::Device::getDeviceQueue(const std::uint32_t       queue_family_index,
                             const std::uint32_t       queue_index,
                             std::unique_ptr<Mutex> && mutex) const
{
    Queue ret{ this->getLabel() + "/Queue"                        //
               + "/Family[" + std::to_string(queue_family_index)  //
               + "]/Index[" + std::to_string(queue_index) + "]" };
    vkGetDeviceQueue(this->_ref(), queue_family_index, queue_index, ret.reset());
    ret.queue_family_index_ = queue_family_index;
    ret.mutex_              = std::move(mutex);
    return ret;
}

vkwp::Buffer
vkwp::Device::createBuffer(const std::string &       label,
                           const VkDeviceSize        size,
                           const VkBufferUsageFlags  usage,
                           const VkBufferCreateFlags flags,
                           const void * const        p_next) const
{
    Buffer                   ret{ label };
    const VkBufferCreateInfo ci{
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = p_next,
        .flags                 = flags,
        .size                  = size,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
    };
    VulkanError::check(  //
        vkCreateBuffer(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateBuffer().");
    return ret;
}

vkwp::Image
vkwp::Device::createImage(const std::string &         label,
                          const VkImageType           image_type,
                          const VkFormat              format,
                          const VkExtent3D            extent,
                          const std::uint32_t         mip_levels,
                          const std::uint32_t         array_layers,
                          const VkSampleCountFlagBits samples,
                          const VkImageTiling         tiling,
                          const VkImageUsageFlags     usage,
                          const VkImageCreateFlags    flags,
                          const void * const          p_next) const
{
    Image                   ret{ label };
    const VkImageCreateInfo ci{
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = p_next,
        .flags                 = flags,
        .imageType             = image_type,
        .format                = format,
        .extent                = extent,
        .mipLevels             = mip_levels,
        .arrayLayers           = array_layers,
        .samples               = samples,
        .tiling                = tiling,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    VulkanError::check(  //
        vkCreateImage(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateImage().");
    return ret;
}

vkwp::ImageView
vkwp::Device::createImageView(const std::string &             label,
                              const VkImage                   image,
                              const VkImageViewType           view_type,
                              const VkFormat                  format,
                              const VkComponentMapping &      components,
                              const VkImageSubresourceRange & subresource_range,
                              const VkImageViewCreateFlags    flags,
                              const void * const              p_next) const
{
    ImageView                   ret{ label };
    const VkImageViewCreateInfo ci{
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext            = p_next,
        .flags            = flags,
        .image            = image,
        .viewType         = view_type,
        .format           = format,
        .components       = components,
        .subresourceRange = subresource_range,
    };
    VulkanError::check(  //
        vkCreateImageView(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateImageView().");
    return ret;
}

vkwp::DeviceMemory
vkwp::Device::allocateDeviceMemory(const std::string &          label,
                                   const PhysicalDevice &       physical_device,
                                   const VkMemoryRequirements & memory_requirements,
                                   const VkMemoryPropertyFlags  memory_property_flags,
                                   const void * const           p_next) const
{
    DeviceMemory ret{ label };
    const auto   memory_type_index
        = physical_device.getMemoryTypeIndex(memory_property_flags, memory_requirements.memoryTypeBits);
    if (!memory_type_index.first)
    {
        SPDLOG_CRITICAL("Available memory is not found. memoryPropertyFlags={}, Required memoryTypeBits={}",
                        memory_property_flags,
                        memory_requirements.memoryTypeBits);
        throw VulkanError{ VK_ERROR_FEATURE_NOT_PRESENT, "Available memory is not found" };
    }

    const VkMemoryAllocateInfo ai{
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = p_next,
        .allocationSize  = memory_requirements.size,
        .memoryTypeIndex = memory_type_index.second,
    };
    SPDLOG_DEBUG("DeviceMemory: {} - {}Bytes, {:.03f}MiB",
                 label,
                 memory_requirements.size,
                 static_cast<double>(memory_requirements.size) / 1024.0 / 1024.0);
    VulkanError::check(  //
        vkAllocateMemory(this->_ref(), &ai, nullptr, ret.reset(this->_ref())),
        "Failed to vkAllocateMemory().");
    return ret;
}

#if defined(WIN32) && WIN32
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
    DeviceMemory ret{ label };

    VkMemoryWin32HandlePropertiesKHR memprops{
        .sType          = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR,
        .pNext          = nullptr,
        .memoryTypeBits = 0,
    };
    const auto vkGetMemoryWin32HandlePropertiesKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(
        vkGetDeviceProcAddr(this->_ref(), "vkGetMemoryWin32HandlePropertiesKHR"));
    if (vkGetMemoryWin32HandlePropertiesKHR == nullptr)
    {
        SPDLOG_CRITICAL(
            "Failed to get vkGetMemoryWin32HandlePropertiesKHR function. Missing VK_KHR_external_memory_win32 extension?");
        throw std::runtime_error{ "Failed to get vkGetMemoryWin32HandlePropertiesKHR function." };
    }
    VulkanError::check(  //
        vkGetMemoryWin32HandlePropertiesKHR(this->_ref(), handle_type, handle, &memprops),
        "Failed to vkGetMemoryWin32HandlePropertiesKHR()");

    const VkMemoryDedicatedAllocateInfo memory_dedicated_ai{
        .sType  = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
        .pNext  = nullptr,
        .image  = image,
        .buffer = buffer,
    };
    const VkImportMemoryWin32HandleInfoKHR import_memory_win32_handle_info_khr{
        .sType      = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR,
        .pNext      = &memory_dedicated_ai,
        .handleType = handle_type,
        .handle     = handle,
        .name       = name,
    };

    return this->allocateDeviceMemory(  //
        label,
        physical_device,
        VkMemoryRequirements{
            .size           = size,
            .alignment      = 1,
            .memoryTypeBits = memprops.memoryTypeBits,
        },
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &import_memory_win32_handle_info_khr);
}
#endif

vkwp::CommandPool
vkwp::Device::createCommandPool(const std::string &            label,
                                const std::uint32_t            queue_family_index,
                                const VkCommandPoolCreateFlags flags,
                                const void * const             p_next) const
{
    CommandPool                   ret{ label };
    const VkCommandPoolCreateInfo ci{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = p_next,
        .flags            = flags,
        .queueFamilyIndex = queue_family_index,
    };
    VulkanError::check(  //
        vkCreateCommandPool(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateCommandPool().");
    return ret;
}

vkwp::Fence
vkwp::Device::createFence(const std::string & label, const VkFenceCreateFlags flags, const void * const p_next) const
{
    Fence                   ret{ label };
    const VkFenceCreateInfo ci{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = p_next,
        .flags = flags,
    };
    VulkanError::check(  //
        vkCreateFence(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateFence().");
    return ret;
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
    RenderPass                   ret{ label };
    const VkRenderPassCreateInfo ci{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = p_next,
        .flags           = flags,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .subpassCount    = static_cast<std::uint32_t>(subpasses.size()),
        .pSubpasses      = subpasses.data(),
        .dependencyCount = static_cast<std::uint32_t>(dependencies.size()),
        .pDependencies   = dependencies.data(),
    };
    VulkanError::check(  //
        vkCreateRenderPass(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateRenderPass()");
    return ret;
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
    Framebuffer                   ret{ label };
    const VkFramebufferCreateInfo ci{
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = p_next,
        .flags           = 0,
        .renderPass      = render_pass,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = width,
        .height          = height,
        .layers          = layers,
    };
    VulkanError::check(  //
        vkCreateFramebuffer(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreateFramebuffer().");
    return ret;
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
    return ret;
}

vkwp::PipelineLayout
vkwp::Device::createPipelineLayout(const std::string &                        label,
                                   const std::vector<VkDescriptorSetLayout> & set_layouts,
                                   const std::vector<VkPushConstantRange> &   push_constant_ranges,
                                   const VkPipelineLayoutCreateFlags          flags,
                                   const void * const                         p_next) const
{
    PipelineLayout                   ret{ label };
    const VkPipelineLayoutCreateInfo ci{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = p_next,
        .flags                  = flags,
        .setLayoutCount         = static_cast<std::uint32_t>(set_layouts.size()),
        .pSetLayouts            = set_layouts.data(),
        .pushConstantRangeCount = static_cast<std::uint32_t>(push_constant_ranges.size()),
        .pPushConstantRanges    = push_constant_ranges.data(),
    };
    VulkanError::check(  //
        vkCreatePipelineLayout(this->_ref(), &ci, nullptr, ret.reset(this->_ref())),
        "Failed to vkCreatePipelineLayout().");
    return ret;
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
    return ret;
}
