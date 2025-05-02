#ifndef VKWP_RENDER_PASS_HPP
#define VKWP_RENDER_PASS_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkRenderPass wrapper.
///
class RenderPass final : public Resource<VkRenderPass, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit RenderPass();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit RenderPass(const std::string & label);
};

}  // namespace vkwp

#endif
