#ifndef VKWP_SAMPLER_HPP
#define VKWP_SAMPLER_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string_view>

namespace vkwp {

///
/// @brief VkSampler wrapper.
///
class Sampler final : public Resource<VkSampler, PFN_vkDestroySampler, VkDevice>
{
public:
    using Resource<VkSampler, PFN_vkDestroySampler, VkDevice>::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next                   = nullptr;
        VkSamplerCreateFlags       flags                    = 0;
        bool                       anisotropy_enable        = false;
        float                      max_anisotropy           = 1.0f;
        VkBorderColor              border_color             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;  // 0, 0, 0, 0
        float                      mip_lod_bias             = 1.0f;
        bool                       compare_enable           = false;
        VkCompareOp                compare_op               = VK_COMPARE_OP_ALWAYS;
        float                      min_lod                  = 0.0f;
        float                      max_lod                  = VK_LOD_CLAMP_NONE;
        bool                       unnormalized_coordinates = false;
        const DebugUtilsResource * debug_utils              = nullptr;
        const VulkanFunctions *    functions                = nullptr;
    };

    static Sampler
    create(const std::string_view     label,
           const Device &             device,
           const VkFilter             mag_filter,
           const VkFilter             min_filter,
           const VkSamplerMipmapMode  mipmap_mode,
           const VkSamplerAddressMode address_mode_u,
           const VkSamplerAddressMode address_mode_v,
           const VkSamplerAddressMode address_mode_w);

    static Sampler
    create(const std::string_view     label,
           const Device &             device,
           const VkFilter             mag_filter,
           const VkFilter             min_filter,
           const VkSamplerMipmapMode  mipmap_mode,
           const VkSamplerAddressMode address_mode_u,
           const VkSamplerAddressMode address_mode_v,
           const VkSamplerAddressMode address_mode_w,
           const OptionalCreateArgs & opt_args);
};

}  // namespace vkwp

#endif
