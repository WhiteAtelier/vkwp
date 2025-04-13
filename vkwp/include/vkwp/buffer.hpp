#ifndef VKWP_BUFFER_HPP
#define VKWP_BUFFER_HPP

#include "fwd.hpp"
#include "resource.hpp"

#include <vulkan/vulkan.h>

namespace vkwp {

///
/// @brief VkBuffer wrapper.
///
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkBuffer.html
///
class Buffer final : public Resource<VkBuffer, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]] explicit Buffer();

    /// @brief Constructor with label name.
    [[nodiscard]] explicit Buffer(const std::string & label);

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
    bindMemory(const VkDeviceMemory & device_memory, const VkDeviceSize offset) const;
};

}  // namespace vkwp

#endif
