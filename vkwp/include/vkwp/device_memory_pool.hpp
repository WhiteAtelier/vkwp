#ifndef VKWP_DEVICE_MEMORY_POOL_HPP
#define VKWP_DEVICE_MEMORY_POOL_HPP

#include "fwd.hpp"
#include "vulkan.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace vkwp {

///
/// @brief Device Memory Pool.
///
class DeviceMemoryPool final
{
public:
    /// @brief Default constructor.
    ///
    /// Create an invalid pool instance.
    [[nodiscard]]
    DeviceMemoryPool();

    /// @brief Move constructor.
    DeviceMemoryPool(DeviceMemoryPool &&) noexcept;

    /// @brief Destructor.
    ~DeviceMemoryPool() noexcept;

    /// @brief Move assigner.
    DeviceMemoryPool &
    operator=(DeviceMemoryPool &&) noexcept;

    DeviceMemoryPool(const DeviceMemoryPool &) = delete;  // Uncopyable
    DeviceMemoryPool &
    operator=(const DeviceMemoryPool &)
        = delete;  // Unassignable

    static DeviceMemoryPool
    create(const std::string &        label,
           const Device &             device,
           const PhysicalDevice &     physical_device,
           const std::size_t          block_size,
           const std::size_t          max_size,
           const void *               allocate_memory_p_next = nullptr,
           const DebugUtilsResource * debug_util             = nullptr);

    /// @brief 有効な Vulkan Resource を持っていないかどうか取得する.
    [[nodiscard]]
    bool
    operator!() const & noexcept;

    /// @brief 有効な Vulkan Resource を持っているかどうか取得する.
    [[nodiscard]]
    explicit
    operator bool() const & noexcept;

    /// @brief Get pool name.
    [[nodiscard]]
    const std::string &
    getLabel() const noexcept;

    /// @brief Get memory unit size. [Bytes]
    ///
    /// ほとんどの場合においてこのサイズで確保するものですが,
    /// 制約上必ずしも全てのメモリをこのサイズで確保しません.
    ///
    /// このユニットサイズ以上のメモリノードも確保可能です.
    /// その場合, そのメモリノードが確保されるメモリはそのノードサイズ以上となり,
    /// メモリノードが解放されても保持されることがあります.
    ///
    /// また, トータルの確保サイズが max memory size 制限に近づき, 残りの確保可能サイズがユニットサイズを下回る場合は
    /// 制限までのサイズで確保します.
    [[nodiscard]]
    std::size_t
    getUnitSize() const noexcept;

    [[nodiscard]]
    DeviceMemoryNode
    allocate(const std::string &          label,
             const VkMemoryPropertyFlags  memory_property_flags,
             const VkMemoryRequirements & memory_requirements);

private:
    friend Device;

    /// @brief Constructor with label name.
    [[nodiscard]]
    DeviceMemoryPool(const std::string &        label,
                     const Device &             device,
                     const PhysicalDevice &     physical_device,
                     const std::size_t          block_size,
                     const std::size_t          max_size,
                     const void *               allocate_memory_p_next = nullptr,
                     const DebugUtilsResource * debug_util             = nullptr);

    using DeviceMemoryBlockPtr = std::unique_ptr<class DeviceMemoryBlock>;

    std::string                                                   label_;
    VkDevice                                                      device_;
    VkPhysicalDeviceMemoryProperties                              memory_properties_;
    std::size_t                                                   block_size_;    // memory unit size [Bytes]
    std::size_t                                                   max_size_;      // max memories size [Bytes]
    std::size_t                                                   current_size_;  // current amount size [Bytes]
    std::uint32_t                                                 unit_index_;
    std::unique_ptr<std::mutex>                                   mutex_;
    std::vector<std::multimap<std::size_t, DeviceMemoryBlockPtr>> memory_blocks_;  // memory instances
    const void *                                                  allocate_memory_p_next_;
    const DebugUtilsResource *                                    debug_util_;
};

}  // namespace vkwp

#endif
