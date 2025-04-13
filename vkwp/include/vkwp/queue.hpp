#ifndef VKWP_QUEUE_HPP
#define VKWP_QUEUE_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace vkwp {

///
/// @brief VkQueue wrapper.
///
class Queue final : public Resource<VkQueue, void, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit Queue();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit Queue(const std::string & label);
};

}  // namespace vkwp

#endif
