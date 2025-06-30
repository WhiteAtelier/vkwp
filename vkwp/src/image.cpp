#include "vkwp/image.hpp"

#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::Image::Image() = default;

vkwp::Image::Image(const std::string & label)
    : Resource{ label }
{}

vkwp::Image
vkwp::Image::create(const std::string &        label,
                    const Device &             device,
                    const VkImageType          image_type,
                    const VkFormat             format,
                    const VkExtent3D           extent,
                    const VkImageUsageFlags    usage,
                    const OptionalCreateArgs & opt_args)
{
    Image                   ret{ label };
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
    VulkanError::check(  //
        vkCreateImage(device, &ci, nullptr, ret.reset(device)),
        "Failed to vkCreateImage().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_IMAGE,
                                                         static_cast<VkImage>(ret),
                                                         label);
    }
    return ret;
}

vkwp::Image
vkwp::Image::create1D(const std::string &        label,
                      const Device &             device,
                      const VkFormat             format,
                      const std::uint32_t        width,
                      const VkImageUsageFlags    usage,
                      const OptionalCreateArgs & opt_args)
{
    return Image::create(label,
                         device,
                         VK_IMAGE_TYPE_1D,
                         format,
                         VkExtent3D{ .width = width, .height = 1u, .depth = 1u },
                         usage,
                         opt_args);
}

vkwp::Image
vkwp::Image::create2D(const std::string &        label,
                      const Device &             device,
                      const VkFormat             format,
                      const std::uint32_t        width,
                      const std::uint32_t        height,
                      const VkImageUsageFlags    usage,
                      const OptionalCreateArgs & opt_args)
{
    return Image::create(label,
                         device,
                         VK_IMAGE_TYPE_2D,
                         format,
                         VkExtent3D{ .width = width, .height = height, .depth = 1u },
                         usage,
                         opt_args);
}

vkwp::Image
vkwp::Image::create3D(const std::string &        label,
                      const Device &             device,
                      const VkFormat             format,
                      const std::uint32_t        width,
                      const std::uint32_t        height,
                      const std::uint32_t        depth,
                      const VkImageUsageFlags    usage,
                      const OptionalCreateArgs & opt_args)
{
    return Image::create(label,
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
    VkMemoryRequirements ret;
    vkGetImageMemoryRequirements(this->_getParent(), this->_ref(), &ret);
    return ret;
}

void
vkwp::Image::bindMemory(const VkDeviceMemory & device_memory, const VkDeviceSize offset) const
{
    VulkanError::check(  //
        vkBindImageMemory(this->_getParent(), this->_ref(), device_memory, offset),
        "Failed to vkBindImageMemory().");
}
