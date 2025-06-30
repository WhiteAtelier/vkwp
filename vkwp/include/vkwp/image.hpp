#ifndef VKWP_IMAGE_HPP
#define VKWP_IMAGE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkImage wrapper.
///
class Image final : public Resource<VkImage, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    Image();

    struct OptionalCreateArgs
    {
        const void *               p_next       = nullptr;
        VkImageCreateFlags         flags        = 0;
        std::uint32_t              mip_levels   = 1;
        std::uint32_t              array_layers = 1;
        VkSampleCountFlagBits      samples      = VK_SAMPLE_COUNT_1_BIT;
        VkImageTiling              tiling       = VK_IMAGE_TILING_OPTIMAL;
        const DebugUtilsResource * debug_utils  = nullptr;
    };

    [[nodiscard]]
    static Image
    create(const std::string &        label,
           const Device &             device,
           const VkImageType          image_type,
           const VkFormat             format,
           const VkExtent3D           extent,
           const VkImageUsageFlags    usage,
           const OptionalCreateArgs & opt_args = {});

    [[nodiscard]]
    static Image
    create1D(const std::string &        label,
             const Device &             device,
             const VkFormat             format,
             const std::uint32_t        width,
             const VkImageUsageFlags    usage,
             const OptionalCreateArgs & opt_args = {});

    [[nodiscard]]
    static Image
    create2D(const std::string &        label,
             const Device &             device,
             const VkFormat             format,
             const std::uint32_t        width,
             const std::uint32_t        height,
             const VkImageUsageFlags    usage,
             const OptionalCreateArgs & opt_args = {});

    [[nodiscard]]
    static Image
    create3D(const std::string &        label,
             const Device &             device,
             const VkFormat             format,
             const std::uint32_t        width,
             const std::uint32_t        height,
             const std::uint32_t        depth,
             const VkImageUsageFlags    usage,
             const OptionalCreateArgs & opt_args = {});

    [[nodiscard]]
    VkMemoryRequirements
    getMemoryRequirements() const;

    void
    bindMemory(const VkDeviceMemory & device_memory, const VkDeviceSize offset) const;

private:
    [[nodiscard]]
    explicit Image(const std::string & label);
};

}  // namespace vkwp

#endif
