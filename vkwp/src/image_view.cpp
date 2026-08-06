#include "vkwp/image_view.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/image.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkImageView i, PFN_vkDestroyImageView fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// ImageView class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::ImageView
vkwp::ImageView::create(std::string                     label,
                        const Device &                  device,
                        const Image &                   image,
                        const VkImageViewType           view_type,
                        const VkFormat                  format,
                        const VkImageSubresourceRange & subresource_range)
{
    return ImageView::create(std::move(label),  //
                             device,
                             image,
                             view_type,
                             format,
                             subresource_range,
                             {});
}

/* STATIC */
vkwp::ImageView
vkwp::ImageView::create(std::string                     label,
                        const Device &                  device,
                        const Image &                   image,
                        const VkImageViewType           view_type,
                        const VkFormat                  format,
                        const VkImageSubresourceRange & subresource_range,
                        const OptionalCreateArgs &      opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(image);

    ImageView                   ret{ std::move(label) };
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
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateImageView != nullptr)
                                ? opt_args.functions->vkCreateImageView
                                : vkCreateImageView;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyImageView != nullptr)
                                ? opt_args.functions->vkDestroyImageView
                                : vkDestroyImageView;

    VulkanError::check(create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),  //
                       "Failed to vkCreateImageView().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_IMAGE_VIEW,
                                                         static_cast<VkImageView>(ret),
                                                         ret.getLabel());
    }
    return ret;
}
