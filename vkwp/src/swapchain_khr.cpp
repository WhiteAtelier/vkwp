#include "vkwp/swapchain_khr.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/surface_khr.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkSwapchainKHR, VkDevice>(VkSwapchainKHR i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroySwapchainKHR(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineLayout class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::SwapchainKHR
vkwp::SwapchainKHR::create(const std::string_view              label,
                           const Device &                      device,
                           const SurfaceKHR &                  surface,
                           const std::uint32_t                 min_image_count,
                           const VkFormat                      image_format,
                           const VkColorSpaceKHR               image_color_space,
                           const VkExtent2D                    image_extent,
                           const VkImageUsageFlags             image_usage,
                           const VkPresentModeKHR              present_mode,
                           const VkSurfaceTransformFlagBitsKHR pre_transform)
{
    return SwapchainKHR::create(label,
                                device,
                                surface,
                                min_image_count,
                                image_format,
                                image_color_space,
                                image_extent,
                                image_usage,
                                present_mode,
                                pre_transform,
                                {});
}

/* STATIC */
vkwp::SwapchainKHR
vkwp::SwapchainKHR::create(const std::string_view              label,
                           const Device &                      device,
                           const SurfaceKHR &                  surface,
                           const std::uint32_t                 min_image_count,
                           const VkFormat                      image_format,
                           const VkColorSpaceKHR               image_color_space,
                           const VkExtent2D                    image_extent,
                           const VkImageUsageFlags             image_usage,
                           const VkPresentModeKHR              present_mode,
                           const VkSurfaceTransformFlagBitsKHR pre_transform,
                           const OptionalCreateArgs &          opt_args)
{
    VKWP_VALIDATE_ARG(device);
    SwapchainKHR                   ret{ label };
    const VkSwapchainCreateInfoKHR ci{
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = opt_args.p_next,
        .flags                 = opt_args.flags,
        .surface               = surface,
        .minImageCount         = min_image_count,
        .imageFormat           = image_format,
        .imageColorSpace       = image_color_space,
        .imageExtent           = image_extent,
        .imageArrayLayers      = opt_args.image_array_layers,
        .imageUsage            = image_usage,
        .imageSharingMode      = opt_args.image_sharing_mode,
        .queueFamilyIndexCount = opt_args.queue_family_index_count,
        .pQueueFamilyIndices   = opt_args.queue_family_indices,
        .preTransform          = pre_transform,
        .compositeAlpha        = opt_args.composite_alpha,
        .presentMode           = present_mode,
        .clipped               = opt_args.clipped,
        .oldSwapchain          = opt_args.old_swapchain != nullptr  //
                                   ? static_cast<VkSwapchainKHR>(*opt_args.old_swapchain)
                                   : nullptr,
    };
    VulkanError::check(  //
        vkCreateSwapchainKHR(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreateSwapchainKHR().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_SWAPCHAIN_KHR,
                                                         static_cast<VkSwapchainKHR>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

void
vkwp::SwapchainKHR::setHdrMetadata(const VkHdrMetadataEXT & metadata)
{
    const auto func = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(
        vkGetDeviceProcAddr(std::get<0>(this->parents_), "vkSetHdrMetadataEXT"));
    if (func == nullptr)
    {
        SPDLOG_CRITICAL("vkSetHdrMetadataEXT is not supported.");
        throw std::runtime_error{ "vkSetHdrMetadataEXT is not supported." };
    }
    func(std::get<0>(this->parents_), 1, this->ptr(), &metadata);
}

std::vector<VkImage>
vkwp::SwapchainKHR::getSwapchainImages() const
{
    std::vector<VkImage> ret;
    std::uint32_t        count = 0;
    VulkanError::check(  //
        vkGetSwapchainImagesKHR(std::get<0>(this->parents_), *this, &count, nullptr),
        "Failed to vkGetSwapchainImagesKHR(). (array count)");
    ret.resize(count);
    VulkanError::check(  //
        vkGetSwapchainImagesKHR(std::get<0>(this->parents_), *this, &count, ret.data()),
        "Failed to vkGetSwapchainImagesKHR(). (array data)");
    return ret;
}
