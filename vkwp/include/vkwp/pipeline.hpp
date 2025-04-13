#ifndef VKWP_PIPELINE_HPP
#define VKWP_PIPELINE_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief VkPipeline wrapper.
///
class Pipeline final : public Resource<VkPipeline, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit Pipeline();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit Pipeline(const std::string & label);
};

}  // namespace vkwp

#endif
