#ifndef VKWP_SURFACE_KHR_HPP
#define VKWP_SURFACE_KHR_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief VkSurfaceKHR wrapper.
///
class SurfaceKHR final : public Resource<VkSurfaceKHR, VkInstance, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit SurfaceKHR();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit SurfaceKHR(const std::string & label);
};

}  // namespace vkwp

#endif
