#ifndef VKWP_SHADER_MODULE_HPP
#define VKWP_SHADER_MODULE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkShaderModule wrapper.
///
class ShaderModule final : public Resource<VkShaderModule, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit ShaderModule();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit ShaderModule(const std::string & label);
};

}  // namespace vkwp

#endif
