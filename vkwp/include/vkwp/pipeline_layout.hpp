#ifndef VKWP_PIPELINE_LAYOUT_HPP
#define VKWP_PIPELINE_LAYOUT_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkPipelineLayout wrapper.
///
class PipelineLayout final : public Resource<VkPipelineLayout, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit PipelineLayout();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit PipelineLayout(const std::string & label);
};

}  // namespace vkwp

#endif
