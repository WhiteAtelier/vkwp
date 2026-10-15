// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "vkwp/image.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/device_memory.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkImage i, PFN_vkDestroyImage fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Image class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Image
vkwp::Image::create(std::string             label,
                    const Device &          device,
                    const VkImageType       image_type,
                    const VkFormat          format,
                    const VkExtent3D        extent,
                    const VkImageUsageFlags usage)
{
    return Image::create(std::move(label), device, image_type, format, extent, usage, {});
}

/* STATIC */
vkwp::Image
vkwp::Image::create(std::string                label,
                    const Device &             device,
                    const VkImageType          image_type,
                    const VkFormat             format,
                    const VkExtent3D           extent,
                    const VkImageUsageFlags    usage,
                    const OptionalCreateArgs & opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_ASSERTION(extent.width >= 1);
    VKWP_ASSERTION(extent.height >= 1);
    VKWP_ASSERTION(extent.depth >= 1);
    Image                   ret{ std::move(label) };
    const VkImageCreateInfo ci{
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = opt_args.p_next,
        .flags                 = opt_args.flags,
        .imageType             = image_type,
        .format                = format,
        .extent                = extent,
        .mipLevels             = opt_args.mip_levels,
        .arrayLayers           = opt_args.array_layers,
        .samples               = opt_args.samples,
        .tiling                = opt_args.tiling,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateImage != nullptr)
                                ? opt_args.functions->vkCreateImage
                                : vkCreateImage;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyImage != nullptr)
                                ? opt_args.functions->vkDestroyImage
                                : vkDestroyImage;

    VulkanError::check(create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),  //
                       "Failed to vkCreateImage().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_IMAGE,
                                                         static_cast<VkImage>(ret),
                                                         ret.getLabel());
    }
    if (opt_args.functions != nullptr)
    {
        ret._vkGetImageMemoryRequirements  = (opt_args.functions->vkGetImageMemoryRequirements != nullptr)
                                               ? opt_args.functions->vkGetImageMemoryRequirements
                                               : vkGetImageMemoryRequirements;
        ret._vkGetImageMemoryRequirements2 = (opt_args.functions->vkGetImageMemoryRequirements2 != nullptr)
                                               ? opt_args.functions->vkGetImageMemoryRequirements2
                                               : vkGetImageMemoryRequirements2;
        ret._vkBindImageMemory             = (opt_args.functions->vkBindImageMemory != nullptr)
                                               ? opt_args.functions->vkBindImageMemory
                                               : vkBindImageMemory;
    }
    else
    {
        ret._vkGetImageMemoryRequirements  = vkGetImageMemoryRequirements;
        ret._vkGetImageMemoryRequirements2 = vkGetImageMemoryRequirements2;
        ret._vkBindImageMemory             = vkBindImageMemory;
    }
    return ret;
}

vkwp::Image
vkwp::Image::importExternal(std::string                   label,
                            const Device &                device,
                            const VkImage                 external,
                            const VulkanFunctions * const functions)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(external);

    Image ret{ std::move(label), external, static_cast<VkDevice>(device) };
    ret._vkGetImageMemoryRequirements  = (functions != nullptr && functions->vkGetImageMemoryRequirements != nullptr)
                                           ? functions->vkGetImageMemoryRequirements
                                           : vkGetImageMemoryRequirements;
    ret._vkGetImageMemoryRequirements2 = (functions != nullptr && functions->vkGetImageMemoryRequirements2 != nullptr)
                                           ? functions->vkGetImageMemoryRequirements2
                                           : vkGetImageMemoryRequirements2;
    ret._vkBindImageMemory             = (functions != nullptr && functions->vkBindImageMemory != nullptr)
                                           ? functions->vkBindImageMemory
                                           : vkBindImageMemory;
    return ret;
}

/* STATIC */
vkwp::Image
vkwp::Image::create1D(std::string             label,
                      const Device &          device,
                      const VkFormat          format,
                      const std::uint32_t     width,
                      const VkImageUsageFlags usage)
{
    return Image::create1D(std::move(label),  //
                           device,
                           format,
                           width,
                           usage,
                           {});
}

/* STATIC */
vkwp::Image
vkwp::Image::create1D(std::string                label,
                      const Device &             device,
                      const VkFormat             format,
                      const std::uint32_t        width,
                      const VkImageUsageFlags    usage,
                      const OptionalCreateArgs & opt_args)
{
    return Image::create(std::move(label),
                         device,
                         VK_IMAGE_TYPE_1D,
                         format,
                         VkExtent3D{ .width = width, .height = 1u, .depth = 1u },
                         usage,
                         opt_args);
}

/* STATIC */
vkwp::Image
vkwp::Image::create2D(std::string             label,
                      const Device &          device,
                      const VkFormat          format,
                      const std::uint32_t     width,
                      const std::uint32_t     height,
                      const VkImageUsageFlags usage)
{
    return Image::create2D(std::move(label),  //
                           device,
                           format,
                           width,
                           height,
                           usage,
                           {});
}

/* STATIC */
vkwp::Image
vkwp::Image::create2D(std::string                label,
                      const Device &             device,
                      const VkFormat             format,
                      const std::uint32_t        width,
                      const std::uint32_t        height,
                      const VkImageUsageFlags    usage,
                      const OptionalCreateArgs & opt_args)
{
    return Image::create(std::move(label),
                         device,
                         VK_IMAGE_TYPE_2D,
                         format,
                         VkExtent3D{ .width = width, .height = height, .depth = 1u },
                         usage,
                         opt_args);
}

/* STATIC */
vkwp::Image
vkwp::Image::create3D(std::string             label,
                      const Device &          device,
                      const VkFormat          format,
                      const std::uint32_t     width,
                      const std::uint32_t     height,
                      const std::uint32_t     depth,
                      const VkImageUsageFlags usage)
{
    return Image::create(std::move(label),
                         device,
                         VK_IMAGE_TYPE_3D,
                         format,
                         VkExtent3D{ .width = width, .height = height, .depth = depth },
                         usage,
                         {});
}

/* STATIC */
vkwp::Image
vkwp::Image::create3D(std::string                label,
                      const Device &             device,
                      const VkFormat             format,
                      const std::uint32_t        width,
                      const std::uint32_t        height,
                      const std::uint32_t        depth,
                      const VkImageUsageFlags    usage,
                      const OptionalCreateArgs & opt_args)
{
    return Image::create(std::move(label),
                         device,
                         VK_IMAGE_TYPE_3D,
                         format,
                         VkExtent3D{ .width = width, .height = height, .depth = depth },
                         usage,
                         opt_args);
}

VkMemoryRequirements
vkwp::Image::getMemoryRequirements() const
{
    VKWP_VALIDATE_THIS;
    VkMemoryRequirements ret;
    this->_vkGetImageMemoryRequirements(std::get<0>(this->parents_), *this, &ret);
    return ret;
}

VkMemoryRequirements2
vkwp::Image::getMemoryRequirements2(const void * const p_next) const
{
    VKWP_VALIDATE_THIS;
    VkImageMemoryRequirementsInfo2 image_memory_requirements_info2{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
        .pNext = p_next,
        .image = *this,
    };
    VkMemoryRequirements2 ret;
    this->_vkGetImageMemoryRequirements2(std::get<0>(this->parents_), &image_memory_requirements_info2, &ret);
    return ret;
}

void
vkwp::Image::bindMemory(const DeviceMemory & device_memory, const VkDeviceSize offset) const
{
    VKWP_VALIDATE_THIS;
    VKWP_VALIDATE_ARG(device_memory);
    VulkanError::check(  //
        this->_vkBindImageMemory(std::get<0>(this->parents_), *this, device_memory, offset),
        "Failed to vkBindImageMemory().");
}
