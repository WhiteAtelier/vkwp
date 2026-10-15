// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_FRAMEBUFFER_HPP
#define VKWP_FRAMEBUFFER_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace vkwp {

///
/// @brief VkFramebuffer wrapper.
///
class Framebuffer final : public Resource<VkFramebuffer, PFN_vkDestroyFramebuffer, VkDevice>
{
public:
    using Resource<VkFramebuffer, PFN_vkDestroyFramebuffer, VkDevice>::Resource;

    [[nodiscard]]
    static Framebuffer
    importExternal(std::string label, const Device & device, const VkFramebuffer external);

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkFramebufferCreateFlags   flags       = 0;
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    static Framebuffer
    create(std::string                                                label,
           const Device &                                             device,
           const RenderPass &                                         render_pass,
           const std::vector<std::reference_wrapper<const ImageView>> attachments,
           const std::uint32_t                                        width,
           const std::uint32_t                                        height,
           const std::uint32_t                                        layers);

    static Framebuffer
    create(std::string                                                label,
           const Device &                                             device,
           const RenderPass &                                         render_pass,
           const std::vector<std::reference_wrapper<const ImageView>> attachments,
           const std::uint32_t                                        width,
           const std::uint32_t                                        height,
           const std::uint32_t                                        layers,
           const OptionalCreateArgs &                                 opt_args);
};

}  // namespace vkwp

#endif
