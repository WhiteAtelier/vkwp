#ifndef VKWP_BUFFER_HPP
#define VKWP_BUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief Wrapper for VkBuffer.
///
/// For more details, see:
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkBuffer.html
///
class Buffer final : public Resource<VkBuffer, VkDevice>
{
public:
    using Resource::Resource;

    struct OptionalCreateArgs
    {
        const void *               p_next      = nullptr;  ///< Pointer to a structure extending this structure.
        VkBufferCreateFlags        flags       = 0;        ///< Specifying additional parameters of the buffer.
        const DebugUtilsResource * debug_utils = nullptr;  ///< Pointer to a debug utils resource instance.
    };

    static const OptionalCreateArgs default_opt_args;

    ///
    /// @brief Creates a new buffer instance.
    ///
    /// @param label
    ///     A label string for debugging purposes.
    ///
    /// @param device
    ///     The logical device used to create the buffer object.
    ///
    /// @param size
    ///     The size of the buffer to be created, in bytes.
    ///
    /// @param usage
    ///     A bitmask of VkBufferUsageFlagBits specifying the allowed usages of the buffer.
    ///
    /// @param opt_args
    ///     Optional arguments for buffer creation.
    ///
    /// @return
    ///     A new Buffer instance.
    ///
    static Buffer
    create(const std::string &        label,
           const Device &             device,
           const VkDeviceSize         size,
           const VkBufferUsageFlags   usage,
           const OptionalCreateArgs & opt_args = default_opt_args);

    ///
    /// @brief Retrieves the memory requirements for this buffer.
    ///
    /// @return
    ///     A structure containing the memory requirements.
    ///     For more details, see:
    ///     https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkMemoryRequirements.html
    ///
    VkMemoryRequirements
    getMemoryRequirements() const;

    ///
    /// @brief Binds device memory to this buffer.
    ///
    /// @param device_memory
    ///     The device memory instance to bind. It must be a valid instance.
    ///
    /// @param offset
    ///     The memory offset, in bytes.
    ///
    void
    bindMemory(const DeviceMemory & device_memory, const VkDeviceSize offset) const;

    ///
    /// @brief Binds DeviceMemoryNode to this buffer.
    ///
    /// @param device_memory_node
    ///     The device memory node to bind.
    ///
    /// @param offset
    ///     The memory offset, in bytes.
    ///
    void
    bindMemory(const DeviceMemoryNode & device_memory_node, const VkDeviceSize offset) const;

    ///
    /// @brief Retrieves the device address of this buffer.
    ///
    /// @remark
    ///     The BufferDeviceAddressFeature must be enabled.
    ///
    /// @return
    ///     The device address of the buffer.
    ///
    VkDeviceAddress
    getBufferDeviceAddress() const;
};

}  // namespace vkwp

#endif
