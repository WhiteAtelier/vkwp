#ifndef VKWP_ACCELERATION_STRUCTURE_HPP
#define VKWP_ACCELERATION_STRUCTURE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>

namespace vkwp {

///
/// @brief VkAccelerationStructureKHR wrapper.
///
/// https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccelerationStructureKHR.html
///
class AccelerationStructureKHR final
    : public Resource<VkAccelerationStructureKHR, PFN_vkDestroyAccelerationStructureKHR, VkDevice>
{
public:
    using Resource<VkAccelerationStructureKHR, PFN_vkDestroyAccelerationStructureKHR, VkDevice>::Resource;

    struct OptionalCreateArgs
    {
        void * const                          p_next         = nullptr;
        VkAccelerationStructureCreateFlagsKHR create_flags   = 0;
        VkDeviceAddress                       device_address = 0;
        const DebugUtilsResource * const      debug_utils    = nullptr;
        const VulkanFunctions * const         functions      = nullptr;
    };

    static AccelerationStructureKHR
    create(std::string                          label,
           const Device &                       device,
           const Buffer &                       buffer,
           const VkDeviceSize                   offset,
           const VkDeviceSize                   size,
           const VkAccelerationStructureTypeKHR type);

    static AccelerationStructureKHR
    create(std::string                          label,
           const Device &                       device,
           const Buffer &                       buffer,
           const VkDeviceSize                   offset,
           const VkDeviceSize                   size,
           const VkAccelerationStructureTypeKHR type,
           const OptionalCreateArgs &           opt_args);

};

}  // namespace vkwp

#endif
