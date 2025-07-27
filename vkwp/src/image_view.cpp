#include "vkwp/image_view.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/image.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkImageView, VkDevice>(VkImageView i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyImageView(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// ImageView class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::ImageView
vkwp::ImageView::create(const std::string &             label,
                        const Device &                  device,
                        const Image &                   image,
                        const VkImageViewType           view_type,
                        const VkFormat                  format,
                        const VkImageSubresourceRange & subresource_range,
                        const OptionalCreateArgs &      opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(image);

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
        vkCreateImageView(device, &ci, nullptr, ret._reset(device)),
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
