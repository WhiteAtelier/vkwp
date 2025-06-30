#include "vkwp/image_view.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/vulkan_error.hpp"

vkwp::ImageView::ImageView() = default;

vkwp::ImageView::ImageView(const std::string & label)
    : Resource{ label }
{}

vkwp::ImageView
vkwp::ImageView::create(const std::string &             label,
                        const VkDevice &                device,
                        const VkImage &                 image,
                        const VkImageViewType           view_type,
                        const VkFormat                  format,
                        const VkImageSubresourceRange & subresource_range,
                        const OptionalCreateArgs &      opt_args)
{
    ImageView                   ret{ label };
    const VkImageViewCreateInfo ci{
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext            = opt_args.p_next,
        .flags            = opt_args.flags,
        .image            = image,
        .viewType         = view_type,
        .format           = format,
        .components       = opt_args.components,
        .subresourceRange = subresource_range,
    };
    VulkanError::check(  //
        vkCreateImageView(device, &ci, nullptr, ret.reset(device)),
        "Failed to vkCreateImageView().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_IMAGE_VIEW,
                                                         static_cast<VkImageView>(ret),
                                                         label);
    }
    return ret;
}
