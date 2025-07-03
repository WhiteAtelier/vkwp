#ifndef VKWP_BUFFER_HPP
#define VKWP_BUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkBuffer wrapper.
///
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

    ///
    /// @brief Create new buffer instance.
    ///
    /// @param label
    ///     Label string for debugging.
    ///
    /// @param device
    ///     The logical device that creates the buffer object.
    ///
    /// @param size
    ///     The size in bytes of the buffer to be created.
    ///
    /// @param usage
    ///     A bitmask of VkBufferUsageFlagBits specifying allowed usages of the buffer.
    ///
    /// @param opt_args
    ///     Optional arguments.
    ///
    /// @return
    ///     Buffer instance.
    ///
    static Buffer
    create(const std::string &        label,
           const Device &             device,
           const VkDeviceSize         size,
           const VkBufferUsageFlags   usage,
           const OptionalCreateArgs & opt_args = {});

    ///
    /// @brief このバッファが要求するデバイスメモリ要件を取得する.
    ///
    /// @return
    ///     メモリ要件についての構造体.
    ///     https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkMemoryRequirements.html
    ///
    VkMemoryRequirements
    getMemoryRequirements() const;

    ///
    /// @brief デバイスメモリをバインドする.
    ///
    /// @param device_memory
    ///     バインドするデバイスメモリインスタンス.
    ///     有効なインスタンスである必要があります.
    ///
    /// @param offset
    ///     メモリオフセット (Bytes).
    ///
    void
    bindMemory(const DeviceMemory & device_memory, const VkDeviceSize offset) const;

    /// @brief このバッファのデバイスアドレスを取得する.
    ///
    /// BufferDeviceAddressFeature を有効にしている必要があります.
    VkDeviceAddress
    getBufferDeviceAddress() const;
};

}  // namespace vkwp

#endif
