#include "vkwp/render_pass.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/render_pass.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkRenderPass, VkDevice>(VkRenderPass i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroyRenderPass(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Image class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::RenderPass
vkwp::RenderPass::create(const std::string &                          label,
                         const Device &                               device,
                         const std::vector<VkAttachmentDescription> & attachments,
                         const std::vector<VkSubpassDescription> &    subpasses,
                         const std::vector<VkSubpassDependency> &     dependencies,
                         const OptionalCreateArgs &                   opt_args)
{
    VKWP_VALIDATE_ARG(device);

    RenderPass                   ret{ label };
    const VkRenderPassCreateInfo ci{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = opt_args.p_next,
        .flags           = opt_args.flags,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .subpassCount    = static_cast<std::uint32_t>(subpasses.size()),
        .pSubpasses      = subpasses.data(),
        .dependencyCount = static_cast<std::uint32_t>(dependencies.size()),
        .pDependencies   = dependencies.data(),
    };
    VulkanError::check(  //
        vkCreateRenderPass(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreateRenderPass()");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_RENDER_PASS,
                                                         static_cast<VkRenderPass>(ret),
                                                         ret.getLabel());
    }
    return ret;
}