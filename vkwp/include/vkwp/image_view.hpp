
#ifndef VKWP_IMAGE_VIEW_HPP
#define VKWP_IMAGE_VIEW_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <string_view>

namespace vkwp {

///
/// @brief VkImageView wrapper.
///
class ImageView final : public Resource<VkImageView, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkImageViewCreateFlags     flags       = 0;
        VkComponentMapping         components  = { .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .a = VK_COMPONENT_SWIZZLE_IDENTITY };
        const DebugUtilsResource * debug_utils = nullptr;
    };

    [[nodiscard]]
    static ImageView
    create(const std::string_view          label,
           const Device &                  device,
           const Image &                   image,
           const VkImageViewType           view_type,
           const VkFormat                  format,
           const VkImageSubresourceRange & subresource_range);

    [[nodiscard]]
    static ImageView
    create(const std::string_view          label,
           const Device &                  device,
           const Image &                   image,
           const VkImageViewType           view_type,
           const VkFormat                  format,
           const VkImageSubresourceRange & subresource_range,
           const OptionalCreateArgs &      opt_args);
};

}  // namespace vkwp

#endif
