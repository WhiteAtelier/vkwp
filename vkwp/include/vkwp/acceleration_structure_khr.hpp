#ifndef VKWP_ACCELERATION_STRUCTURE_HPP
#define VKWP_ACCELERATION_STRUCTURE_HPP

#include "fwd.hpp"

#include "resource.hpp"
#include "vulkan.hpp"

#include <string>
#include <string_view>

namespace vkwp {

struct AccelerationStructureKHR_ExtFunctions
{
    static AccelerationStructureKHR_ExtFunctions
    create(const VkDevice device, const VulkanFunctions * functions = nullptr);

    PFN_vkCreateAccelerationStructureKHR  vkCreateAccelerationStructureKHR  = nullptr;
    PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR = nullptr;
};

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

    static AccelerationStructureKHR
    createEXT(std::string                                   label,
              const Device &                                device,
              const Buffer &                                buffer,
              const VkDeviceSize                            offset,
              const VkDeviceSize                            size,
              const VkAccelerationStructureTypeKHR          type,
              const AccelerationStructureKHR_ExtFunctions & ext_functions);

    static AccelerationStructureKHR
    createEXT(std::string                                   label,
              const Device &                                device,
              const Buffer &                                buffer,
              const VkDeviceSize                            offset,
              const VkDeviceSize                            size,
              const VkAccelerationStructureTypeKHR          type,
              const AccelerationStructureKHR_ExtFunctions & ext_functions,
              const OptionalCreateArgs &                    opt_args);
};

}  // namespace vkwp

#endif
