// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_RENDER_PASS_HPP
#define VKWP_RENDER_PASS_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>
#include <vector>

namespace vkwp {

///
/// @brief VkRenderPass wrapper.
///
class RenderPass final : public Resource<VkRenderPass, PFN_vkDestroyRenderPass, VkDevice>
{
public:
    using Resource<VkRenderPass, PFN_vkDestroyRenderPass, VkDevice>::Resource;

    [[nodiscard]]
    static RenderPass
    importExternal(std::string label, const Device & device, const VkRenderPass external);

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkRenderPassCreateFlags    flags       = 0;
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    static RenderPass
    create(std::string                                  label,
           const Device &                               device,
           const std::vector<VkAttachmentDescription> & attachments,
           const std::vector<VkSubpassDescription> &    subpasses,
           const std::vector<VkSubpassDependency> &     dependencies);

    static RenderPass
    create(std::string                                  label,
           const Device &                               device,
           const std::vector<VkAttachmentDescription> & attachments,
           const std::vector<VkSubpassDescription> &    subpasses,
           const std::vector<VkSubpassDependency> &     dependencies,
           const OptionalCreateArgs &                   opt_args);
};

}  // namespace vkwp

#endif
