// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#include "vkwp/render_pass.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"

#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
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
vkwp::impl::_delete(VkRenderPass i, PFN_vkDestroyRenderPass fn, const std::tuple<VkDevice> & args) noexcept
{
    fn(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Image class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
vkwp::RenderPass
vkwp::RenderPass::importExternal(std::string label, const Device & device, const VkRenderPass external)
{
    VKWP_VALIDATE_ARG(device);
    VKWP_VALIDATE_ARG(external);
    return RenderPass{ std::move(label), external, static_cast<VkDevice>(device) };
}

/* STATIC */
vkwp::RenderPass
vkwp::RenderPass::create(std::string                                  label,
                         const Device &                               device,
                         const std::vector<VkAttachmentDescription> & attachments,
                         const std::vector<VkSubpassDescription> &    subpasses,
                         const std::vector<VkSubpassDependency> &     dependencies)
{
    return RenderPass::create(std::move(label), device, attachments, subpasses, dependencies, {});
}

/* STATIC */
vkwp::RenderPass
vkwp::RenderPass::create(std::string                                  label,
                         const Device &                               device,
                         const std::vector<VkAttachmentDescription> & attachments,
                         const std::vector<VkSubpassDescription> &    subpasses,
                         const std::vector<VkSubpassDependency> &     dependencies,
                         const OptionalCreateArgs &                   opt_args)
{
    VKWP_VALIDATE_ARG(device);

    RenderPass                   ret{ std::move(label) };
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
    auto * const create_fn  = (opt_args.functions != nullptr && opt_args.functions->vkCreateRenderPass != nullptr)
                                ? opt_args.functions->vkCreateRenderPass
                                : vkCreateRenderPass;
    auto * const destroy_fn = (opt_args.functions != nullptr && opt_args.functions->vkDestroyRenderPass != nullptr)
                                ? opt_args.functions->vkDestroyRenderPass
                                : vkDestroyRenderPass;

    VulkanError::check(  //
        create_fn(device, &ci, nullptr, ret._reset(destroy_fn, device)),
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
