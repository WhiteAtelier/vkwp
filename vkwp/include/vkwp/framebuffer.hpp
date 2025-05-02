#ifndef VKWP_FRAMEBUFFER_HPP
#define VKWP_FRAMEBUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkFramebuffer wrapper.
///
class Framebuffer final : public Resource<VkFramebuffer, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit Framebuffer();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit Framebuffer(const std::string & label);
};

}  // namespace vkwp

#endif
