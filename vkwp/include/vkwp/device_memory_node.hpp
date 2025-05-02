#ifndef VKWP_DEVICE_MEMORY_NODE_HPP
#define VKWP_DEVICE_MEMORY_NODE_HPP

#include "fwd.hpp"
#include "vulkan.hpp"

#include <string>

namespace vkwp {

namespace impl {
class DeviceMemoryPoolUnit;
}  // namespace impl

class DeviceMemoryNode final
{
public:
    [[nodiscard]]
    explicit DeviceMemoryNode();

    [[nodiscard]]
    explicit DeviceMemoryNode(const std::string & label);

    DeviceMemoryNode(DeviceMemoryNode &&) noexcept;
    ~DeviceMemoryNode() noexcept;

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

    /// @brief VkMemoryRequirements を取得する.
    [[nodiscard]]
    const VkMemoryRequirements &
    getMemoryRequirements() const & noexcept;

    /// @brief ノードを解放する.
    ///
    /// 解放後は invalid node になります.
    void
    reset() & noexcept;

    /// @brief ノードオフセット.
    [[nodiscard]]
    std::size_t
    getNodeOffset() const & noexcept;

    void
    write(const void * const src, const std::size_t size, const std::size_t offset = 0);

    void
    read(void * const dst, const std::size_t size, const std::size_t offset = 0);

    // clang-format off
    bool operator!() && = delete;
    explicit operator bool() && = delete;
    const std::string & getLabel() && = delete;
    const VkMemoryRequirements & getMemoryRequirements() && = delete;
    void reset() && = delete;
    std::size_t getNodeOffset() && = delete;
    std::size_t getNodeSize() && = delete;
    // clang-format on

private:
    friend impl::DeviceMemoryPoolUnit;

    [[nodiscard]]
    explicit DeviceMemoryNode(const std::string &                label,
                              impl::DeviceMemoryPoolUnit * const unit,
                              void *                             node_ptr,
                              const VkMemoryRequirements &       memory_requirements,
                              const std::size_t                  node_offset);

    std::string                  label_;
    impl::DeviceMemoryPoolUnit * unit_;
    void *                       node_ptr_;
    VkMemoryRequirements         memory_requirements_;
    std::size_t                  node_offset_;
};

}  // namespace vkwp

#endif