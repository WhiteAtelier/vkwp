#ifndef VKWP_FENCE_HPP
#define VKWP_FENCE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

#include <limits>

namespace vkwp {

///
/// @brief VkFence wrapper.
///
class Fence final : public Resource<VkFence, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit Fence();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit Fence(const std::string & label);

    bool
    wait(const std::uint64_t timeout = (std::numeric_limits<std::uint64_t>::max)()) const;

    void
    resetSignal() const;
};

}  // namespace vkwp

#endif
