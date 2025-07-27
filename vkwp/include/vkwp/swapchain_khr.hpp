#ifndef VKWP_SWAPCHAIN_HPP
#define VKWP_SWAPCHAIN_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <vector>

namespace vkwp {

///
/// @brief VkSwapchainKHR wrapper.
///
class SwapchainKHR final : public Resource<VkSwapchainKHR, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *                  p_next                   = nullptr;
        VkSwapchainCreateFlagsKHR     flags                    = 0;
        std::uint32_t                 image_array_layers       = 0;
        VkSharingMode                 image_sharing_mode       = VK_SHARING_MODE_EXCLUSIVE;
        uint32_t                      queue_family_index_count = 0;
        const uint32_t *              queue_family_indices     = nullptr;
        VkSurfaceTransformFlagBitsKHR pre_transform            = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR   composite_alpha          = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        bool                          clipped                  = true;
        const SwapchainKHR *          old_swapchain            = nullptr;
        const DebugUtilsResource *    debug_utils              = nullptr;
    };

    [[nodiscard]]
    static SwapchainKHR
    create(const std::string &        label,
           const Device &             device,
           const SurfaceKHR &         surface,
           const std::uint32_t        min_image_count,
           const VkFormat             image_format,
           const VkColorSpaceKHR      image_color_space,
           const VkExtent2D           image_extent,
           const VkImageUsageFlags    image_usage,
           const VkPresentModeKHR     present_mode,
           const OptionalCreateArgs & opt_args = {});

    // VK_EXT_hdr_metadata 拡張が必要.
    void
    setHdrMetadata(const VkHdrMetadataEXT & metadata);

    [[nodiscard]]
    std::vector<VkImage>
    getSwapchainImages() const;
};

}  // namespace vkwp

#endif
