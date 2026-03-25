#ifndef VKWP_FRAMEBUFFER_HPP
#define VKWP_FRAMEBUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <cstdint>
#include <type_traits>
#include <vector>

namespace vkwp {

///
/// @brief VkFramebuffer wrapper.
///
class Framebuffer final : public Resource<VkFramebuffer, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;
        VkFramebufferCreateFlags   flags       = 0;
        const DebugUtilsResource * debug_utils = nullptr;
    };

    static Framebuffer
    create(const std::string_view                                     label,
           const Device &                                             device,
           const RenderPass &                                         render_pass,
           const std::vector<std::reference_wrapper<const ImageView>> attachments,
           const std::uint32_t                                        width,
           const std::uint32_t                                        height,
           const std::uint32_t                                        layers);

    static Framebuffer
    create(const std::string_view                                     label,
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
