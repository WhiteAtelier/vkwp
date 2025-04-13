#ifndef VKWP_SEMAPHORE_HPP
#define VKWP_SEMAPHORE_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief VkSemaphore wrapper.
///
class Semaphore final : public Resource<VkSemaphore, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit Semaphore();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit Semaphore(const std::string & label);
};

}  // namespace vkwp

#endif
