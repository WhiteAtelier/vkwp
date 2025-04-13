#ifndef VKWP_SAMPLER_HPP
#define VKWP_SAMPLER_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief VkSampler wrapper.
///
class Sampler final : public Resource<VkSampler, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit Sampler();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit Sampler(const std::string & label);
};

}  // namespace vkwp

#endif
