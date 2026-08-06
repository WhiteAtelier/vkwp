#ifndef VKWP_SWAPCHAIN_HPP
#define VKWP_SWAPCHAIN_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace vkwp {

///
/// @brief VkSwapchainKHR wrapper.
///
class SwapchainKHR final : public Resource<VkSwapchainKHR, PFN_vkDestroySwapchainKHR, VkDevice>
{
public:
    using Resource<VkSwapchainKHR, PFN_vkDestroySwapchainKHR, VkDevice>::Resource;

    struct OptionalCreateArgs
    {
        const void *                p_next                   = nullptr;
        VkSwapchainCreateFlagsKHR   flags                    = 0;
        std::uint32_t               image_array_layers       = 1u;
        VkSharingMode               image_sharing_mode       = VK_SHARING_MODE_EXCLUSIVE;
        uint32_t                    queue_family_index_count = 0;
        const uint32_t *            queue_family_indices     = nullptr;
        VkCompositeAlphaFlagBitsKHR composite_alpha          = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        bool                        clipped                  = true;
        const SwapchainKHR *        old_swapchain            = nullptr;
        const DebugUtilsResource *  debug_utils              = nullptr;
        const VulkanFunctions *     functions                = nullptr;
    };

    [[nodiscard]]
    static SwapchainKHR
    create(std::string                         label,
           const Device &                      device,
           const SurfaceKHR &                  surface,
           const std::uint32_t                 min_image_count,
           const VkFormat                      image_format,
           const VkColorSpaceKHR               image_color_space,
           const VkExtent2D                    image_extent,
           const VkImageUsageFlags             image_usage,
           const VkPresentModeKHR              present_mode,
           const VkSurfaceTransformFlagBitsKHR pre_transform);

    [[nodiscard]]
    static SwapchainKHR
    create(std::string                         label,
           const Device &                      device,
           const SurfaceKHR &                  surface,
           const std::uint32_t                 min_image_count,
           const VkFormat                      image_format,
           const VkColorSpaceKHR               image_color_space,
           const VkExtent2D                    image_extent,
           const VkImageUsageFlags             image_usage,
           const VkPresentModeKHR              present_mode,
           const VkSurfaceTransformFlagBitsKHR pre_transform,
           const OptionalCreateArgs &          opt_args);

    // VK_EXT_hdr_metadata 拡張が必要.
    void
    setHdrMetadata(const VkHdrMetadataEXT & metadata);

    [[nodiscard]]
    std::vector<VkImage>
    getSwapchainImages() const;

private:
    PFN_vkSetHdrMetadataEXT     _vkSetHdrMetadataEXT     = nullptr;
    PFN_vkGetSwapchainImagesKHR _vkGetSwapchainImagesKHR = nullptr;
};

}  // namespace vkwp

#endif
