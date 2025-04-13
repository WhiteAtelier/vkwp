#ifndef VKWP_SWAPCHAIN_HPP
#define VKWP_SWAPCHAIN_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace vkwp {

///
/// @brief VkSwapchainKHR wrapper.
///
class SwapchainKHR final : public Resource<VkSwapchainKHR, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit SwapchainKHR();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit SwapchainKHR(const std::string & label);

#if defined(VK_EXT_hdr_metadata) && VK_EXT_hdr_metadata
    void
    setHdrMetadata(const VkHdrMetadataEXT & metadata);
#endif

    [[nodiscard]]
    std::vector<VkImage>
    getSwapchainImages() const;
};

}  // namespace vkwp

#endif
