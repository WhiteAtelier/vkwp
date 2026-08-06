#include "vkwp/framebuffer.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/image_view.hpp"
#include "vkwp/render_pass.hpp"
#include "vkwp/vulkan_error.hpp"
#include "vkwp/vulkan_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete(VkFramebuffer i, PFN_vkDestroyFramebuffer fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Image class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Framebuffer
vkwp::Framebuffer::create(const std::string_view                                     label,
                          const Device &                                             device,
                          const RenderPass &                                         render_pass,
                          const std::vector<std::reference_wrapper<const ImageView>> _attachments,
                          const std::uint32_t                                        width,
                          const std::uint32_t                                        height,
                          const std::uint32_t                                        layers)
{
    return Framebuffer::create(label,  //
                               device,
                               render_pass,
                               _attachments,
                               width,
                               height,
                               layers,
                               {});
}

/* STATIC */
vkwp::Framebuffer
vkwp::Framebuffer::create(const std::string_view                                     label,
                          const Device &                                             device,
                          const RenderPass &                                         render_pass,
                          const std::vector<std::reference_wrapper<const ImageView>> _attachments,
                          const std::uint32_t                                        width,
                          const std::uint32_t                                        height,
                          const std::uint32_t                                        layers,
                          const OptionalCreateArgs &                                 opt_args)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(render_pass);

    std::vector<VkImageView> attachments;
    attachments.reserve(_attachments.size());
    for (const auto & attachment : _attachments)
    {
        attachments.emplace_back(attachment.get());
    }

    Framebuffer                   ret{ label };
    const VkFramebufferCreateInfo ci{
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = opt_args.p_next,
        .flags           = opt_args.flags,
        .renderPass      = render_pass,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = width,
        .height          = height,
        .layers          = layers,
    };
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateFramebuffer != nullptr)
                                ? opt_args.functions->vkCreateFramebuffer
                                : vkCreateFramebuffer;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyFramebuffer != nullptr)
                                ? opt_args.functions->vkDestroyFramebuffer
                                : vkDestroyFramebuffer;

    VulkanError::check(create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),  //
                       "Failed to vkCreateFramebuffer().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_FRAMEBUFFER,
                                                         static_cast<VkFramebuffer>(ret),
                                                         ret.getLabel());
    }
    return ret;
}
