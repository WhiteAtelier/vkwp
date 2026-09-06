#include "vkwp/swapchain_khr.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/surface_khr.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

#include <spdlog/spdlog.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkSwapchainKHR i, PFN_vkDestroySwapchainKHR fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineLayout class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::SwapchainKHR
vkwp::SwapchainKHR::create(std::string                         label,
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
    return SwapchainKHR::create(std::move(label),
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
vkwp::SwapchainKHR::create(std::string                         label,
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
    SwapchainKHR                   ret{ std::move(label) };
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
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateSwapchainKHR != nullptr)
                                ? opt_args.functions->vkCreateSwapchainKHR
                                : vkCreateSwapchainKHR;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroySwapchainKHR != nullptr)
                                ? opt_args.functions->vkDestroySwapchainKHR
                                : vkDestroySwapchainKHR;

    VulkanError::check(  //
        create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),
        "Failed to vkCreateSwapchainKHR().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_SWAPCHAIN_KHR,
                                                         static_cast<VkSwapchainKHR>(ret),
                                                         ret.getLabel());
    }

    if (opt_args.functions != nullptr)
    {
        if (opt_args.functions->vkSetHdrMetadataEXT != nullptr)
        {
            ret._vkSetHdrMetadataEXT = opt_args.functions->vkSetHdrMetadataEXT;
        }
        else
        {
            auto * const get_device_proc_addr_fn = (opt_args.functions->vkGetDeviceProcAddr != nullptr)
                                                     ? opt_args.functions->vkGetDeviceProcAddr
                                                     : vkGetDeviceProcAddr;
            ret._vkSetHdrMetadataEXT
                = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(get_device_proc_addr_fn(device, "vkSetHdrMetadataEXT"));
        }
        ret._vkGetSwapchainImagesKHR = (opt_args.functions->vkGetSwapchainImagesKHR != nullptr)
                                         ? opt_args.functions->vkGetSwapchainImagesKHR
                                         : vkGetSwapchainImagesKHR;
    }
    else
    {
        ret._vkSetHdrMetadataEXT
            = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(vkGetDeviceProcAddr(device, "vkSetHdrMetadataEXT"));
        ret._vkGetSwapchainImagesKHR = vkGetSwapchainImagesKHR;
    }

    return ret;
}

vkwp::SwapchainKHR
vkwp::SwapchainKHR::importExternal(std::string                   label,
                                   const Device &                device,
                                   const VkSwapchainKHR          external,
                                   const VulkanFunctions * const functions)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(external);

    SwapchainKHR ret{ std::move(label), external, static_cast<VkDevice>(device) };
    auto * const get_device_proc_addr_fn = (functions != nullptr && functions->vkGetDeviceProcAddr != nullptr)
                                             ? functions->vkGetDeviceProcAddr
                                             : vkGetDeviceProcAddr;
    ret._vkSetHdrMetadataEXT
        = (functions != nullptr && functions->vkSetHdrMetadataEXT != nullptr)
            ? functions->vkSetHdrMetadataEXT
            : reinterpret_cast<PFN_vkSetHdrMetadataEXT>(get_device_proc_addr_fn(device, "vkSetHdrMetadataEXT"));
    ret._vkGetSwapchainImagesKHR = (functions != nullptr && functions->vkGetSwapchainImagesKHR != nullptr)
                                     ? functions->vkGetSwapchainImagesKHR
                                     : vkGetSwapchainImagesKHR;
    return ret;
}

void
vkwp::SwapchainKHR::setHdrMetadata(const VkHdrMetadataEXT & metadata)
{
    VKWP_VALIDATE_THIS;
    if (this->_vkSetHdrMetadataEXT == nullptr)
    {
        SPDLOG_CRITICAL("vkSetHdrMetadataEXT is not supported.");
        throw std::runtime_error{ "vkSetHdrMetadataEXT is not supported." };
    }
    this->_vkSetHdrMetadataEXT(std::get<0>(this->parents_), 1, this->ptr(), &metadata);
}

std::vector<VkImage>
vkwp::SwapchainKHR::getSwapchainImages() const
{
    VKWP_VALIDATE_THIS;
    std::vector<VkImage>    ret;
    constexpr std::uint32_t max_attempt_count = 10;
    for (std::uint32_t attempt_count = 0; attempt_count < max_attempt_count; ++attempt_count)
    {
        std::uint32_t count = 0;
        VulkanError::check(  //
            this->_vkGetSwapchainImagesKHR(std::get<0>(this->parents_), *this, &count, nullptr),
            "Failed to vkGetSwapchainImagesKHR(). (array count)");
        ret.resize(count);
        const VkResult result = this->_vkGetSwapchainImagesKHR(std::get<0>(this->parents_), *this, &count, ret.data());
        if (result == VK_INCOMPLETE && attempt_count + 1 < max_attempt_count)
        {
            continue;
        }
        VulkanError::check(result, "Failed to vkGetSwapchainImagesKHR(). (array data)");
        ret.resize(count);
        return ret;
    }
    throw VulkanError{ VK_INCOMPLETE, "Failed to vkGetSwapchainImagesKHR() after maximum attempts." };
}
