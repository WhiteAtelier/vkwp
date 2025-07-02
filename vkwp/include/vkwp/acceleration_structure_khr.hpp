#ifndef VKWP_ACCELERATION_STRUCTURE_HPP
#define VKWP_ACCELERATION_STRUCTURE_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

struct AccelerationStructureKHR_ExtFunctions
{
    static AccelerationStructureKHR_ExtFunctions
    create(const VkInstance instance);

    PFN_vkCreateAccelerationStructureKHR  vkCreateAccelerationStructureKHR  = nullptr;
    PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR = nullptr;
};

///
/// @brief VkAccelerationStructureKHR wrapper.
///
/// https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccelerationStructureKHR.html
///
class AccelerationStructureKHR final
    : public Resource<VkAccelerationStructureKHR, VkDevice, AccelerationStructureKHR_ExtFunctions>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        VkAccelerationStructureCreateFlagsKHR create_flags   = 0;
        VkDeviceAddress                       device_address = 0;
        void * const                          p_next         = nullptr;
        const DebugUtilsResource * const      debug_utils    = nullptr;
    };

    static AccelerationStructureKHR
    create(const std::string &                  label,
           const Device &                       device,
           const Buffer &                       buffer,
           const VkDeviceSize                   offset,
           const VkDeviceSize                   size,
           const VkAccelerationStructureTypeKHR type,
           const OptionalCreateArgs &           opt_args = {});

    static AccelerationStructureKHR
    createEXT(const std::string &                           label,
              const Device &                                device,
              const Buffer &                                buffer,
              const VkDeviceSize                            offset,
              const VkDeviceSize                            size,
              const VkAccelerationStructureTypeKHR          type,
              const AccelerationStructureKHR_ExtFunctions & ext_functions,
              const OptionalCreateArgs &                    opt_args = {});
};

}  // namespace vkwp

#endif
