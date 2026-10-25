// Copyright 2026 Shirao Shotaro, WhiteAtelier
// SPDX-License-Identifier: MIT
//
// See the LICENSE file in the project root for the full license text.

#ifndef VKWP_IMAGE_VIEW_HPP
#define VKWP_IMAGE_VIEW_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>

namespace vkwp {

///
/// @brief VkImageView wrapper.
///
class ImageView final : public Resource<VkImageView, PFN_vkDestroyImageView, VkDevice>
{
public:
    using Resource<VkImageView, PFN_vkDestroyImageView, VkDevice>::Resource;

    [[nodiscard]]
    static ImageView
    importExternal(std::string label, const Device & device, const VkImageView external);

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkImageViewCreateFlags     flags       = 0;
        VkComponentMapping         components  = { .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .a = VK_COMPONENT_SWIZZLE_IDENTITY };
        const DebugUtilsResource * debug_utils = nullptr;
        const VulkanFunctions *    functions   = nullptr;
    };

    [[nodiscard]]
    static ImageView
    create(std::string                     label,
           const Device &                  device,
           const VkImage &                 image,
           const VkImageViewType           view_type,
           const VkFormat                  format,
           const VkImageSubresourceRange & subresource_range);

    [[nodiscard]]
    static ImageView
    create(std::string                     label,
           const Device &                  device,
           const VkImage &                 image,
           const VkImageViewType           view_type,
           const VkFormat                  format,
           const VkImageSubresourceRange & subresource_range,
           const OptionalCreateArgs &      opt_args);
};

}  // namespace vkwp

#endif
