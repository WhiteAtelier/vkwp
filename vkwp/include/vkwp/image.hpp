#ifndef VKWP_IMAGE_HPP
#define VKWP_IMAGE_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief VkImage wrapper.
///
class Image final : public Resource<VkImage, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit Image();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit Image(const std::string & label);

    [[nodiscard]]
    VkMemoryRequirements
    getMemoryRequirements() const;

    void
    bindMemory(const VkDeviceMemory & device_memory, const VkDeviceSize offset) const;
};

}  // namespace vkwp

#endif
