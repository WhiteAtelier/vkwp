#ifndef VKWP_DEVICE_MEMORY_HPP
#define VKWP_DEVICE_MEMORY_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkDeviceMemory wrapper.
///
class DeviceMemory final : public Resource<VkDeviceMemory, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit DeviceMemory();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit DeviceMemory(const std::string & label);

    void
    write(const void * const src, const std::size_t size, const std::size_t offset = 0) const;

    void
    read(void * const dst, const std::size_t size, const std::size_t offset = 0) const;
};

}  // namespace vkwp

#endif
