#ifndef VKWP_IMAGE_HPP
#define VKWP_IMAGE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkImage wrapper.
///
class Image final : public Resource<VkImage, VkDevice>
{
public:
    using Resource::Resource;

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

    [[nodiscard]]
    VkMemoryRequirements2
    getMemoryRequirements2(const void * const p_next = nullptr) const;

    void
    bindMemory(const DeviceMemory & device_memory, const VkDeviceSize offset) const;
};

}  // namespace vkwp

#endif
