#ifndef VKWP_DEVICE_MEMORY_NODE_HPP
#define VKWP_DEVICE_MEMORY_NODE_HPP

#include "fwd.hpp"
#include "vulkan.hpp"

#include <string>

namespace vkwp {

class DeviceMemoryNode final
{
public:
    [[nodiscard]]
    explicit DeviceMemoryNode();

    [[nodiscard]]
    explicit DeviceMemoryNode(const std::string & label);

    DeviceMemoryNode(DeviceMemoryNode &&) noexcept;
    ~DeviceMemoryNode() noexcept;

    /// @brief Move assigner.
    DeviceMemoryNode &
    operator=(DeviceMemoryNode &&) noexcept;

    DeviceMemoryNode(const DeviceMemoryNode &) = delete;  // Uncopyable
    DeviceMemoryNode &
    operator=(const DeviceMemoryNode &)
        = delete;  // Unassignable

    /// @brief 有効な Vulkan Resource を持っていないかどうか取得する.
    [[nodiscard]]
    bool
    operator!() const & noexcept;

    /// @brief 有効な Vulkan Resource を持っているかどうか取得する.
    [[nodiscard]]
    explicit
    operator bool() const & noexcept;

    /// @brief ラベル文字列を取得する.
    [[nodiscard]]
    const std::string &
    getLabel() const & noexcept;

    /// @brief ノードを解放する.
    ///
    /// 解放後は invalid node になります.
    void
    reset() & noexcept;

    /// @brief ノードサイズ.
    std::size_t
    getSize() const & noexcept;

    void
    write(const void * const src, const std::size_t size, const std::size_t offset = 0);

    void
    read(void * const dst, const std::size_t size, const std::size_t offset = 0);

    // clang-format off
    bool operator!() && = delete;
    explicit operator bool() && = delete;
    const std::string & getLabel() && = delete;
    void reset() && = delete;
    std::size_t getSize() && = delete;
    // clang-format on

private:
    friend class DeviceMemoryBlock;
    friend Buffer;
    friend Image;

    [[nodiscard]]
    explicit DeviceMemoryNode(const std::string &                 label,
                              class DeviceMemoryBlock * const     block,
                              class DeviceMemoryBlockNode * const node);

    VkDeviceMemory
    _getDeviceMemory() const;

    std::size_t
    _getOffset() const;

    std::string                   label_;
    class DeviceMemoryBlock *     block_;
    class DeviceMemoryBlockNode * node_;
};

}  // namespace vkwp

#endif