#include "vkwp/sampler.hpp"

#include "impl/assert.hpp"
#include "impl/deleter.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device.hpp"
#include "vkwp/vulkan_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// _delete() overload
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void
vkwp::impl::_delete<VkSampler, VkDevice>(VkSampler i, const std::tuple<VkDevice> & args) noexcept
{
    vkDestroySampler(std::get<0>(args), i, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Sampler class
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/* STATIC */
vkwp::Sampler
vkwp::Sampler::create(const std::string &        label,
                      const Device &             device,
                      const VkFilter             mag_filter,
                      const VkFilter             min_filter,
                      const VkSamplerMipmapMode  mipmap_mode,
                      const VkSamplerAddressMode address_mode_u,
                      const VkSamplerAddressMode address_mode_v,
                      const VkSamplerAddressMode address_mode_w,
                      const OptionalCreateArgs & opt_args)
{
    Sampler                   ret{ label };
    const VkSamplerCreateInfo ci{
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = opt_args.p_next,
        .flags                   = opt_args.flags,
        .magFilter               = mag_filter,
        .minFilter               = min_filter,
        .mipmapMode              = mipmap_mode,
        .addressModeU            = address_mode_u,
        .addressModeV            = address_mode_v,
        .addressModeW            = address_mode_w,
        .mipLodBias              = opt_args.mip_lod_bias,
        .anisotropyEnable        = opt_args.anisotropy_enable,
        .maxAnisotropy           = opt_args.max_anisotropy,
        .compareEnable           = opt_args.compare_enable,
        .compareOp               = opt_args.compare_op,
        .minLod                  = opt_args.min_lod,
        .maxLod                  = opt_args.max_lod,
        .borderColor             = opt_args.border_color,
        .unnormalizedCoordinates = opt_args.unnormalized_coordinates,
    };
    VulkanError::check(  //
        vkCreateSampler(device, &ci, nullptr, ret._reset(device)),
        "Failed to vkCreateSampler().");
    if (opt_args.debug_utils != nullptr)
    {
        opt_args.debug_utils->registDebugUtilsObjectName(device,
                                                         VK_OBJECT_TYPE_SAMPLER,
                                                         static_cast<VkSampler>(ret),
                                                         ret.getLabel());
    }
    return ret;
}

const vkwp::Sampler::OptionalCreateArgs vkwp::Sampler::default_opt_args;