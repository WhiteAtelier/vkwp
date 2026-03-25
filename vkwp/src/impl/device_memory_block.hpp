#ifndef VKWP_IMPL_DEVICE_MEMORY_BLOCK_HPP
#define VKWP_IMPL_DEVICE_MEMORY_BLOCK_HPP

#include "vkwp/fwd.hpp"
#include "vkwp/resource.hpp"

#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>

/// @cond
namespace vkwp {

class DeviceMemoryBlockNode;
class DeviceMemoryBlock : public Resource<VkDeviceMemory, VkDevice>
{
public:
    // 必ず allocate() で確保すること.
    DeviceMemoryBlock(const std::string & label, const std::size_t size) noexcept;

    DeviceMemoryBlock(const DeviceMemoryBlock &) = delete;
    DeviceMemoryBlock(DeviceMemoryBlock &&)      = delete;

    ~DeviceMemoryBlock() noexcept;

    /// @brief Device memory block をひとつ確保する.
    ///
    /// @param size
    ///     Memory block のサイズ.
    static std::unique_ptr<DeviceMemoryBlock>
    allocate(const std::string &        label,
             const VkDevice             device,
             const std::uint32_t        memory_type_index,
             const std::size_t          size,
             const bool                 host_visible,
             const void * const         allocate_memory_p_next = nullptr,
             const DebugUtilsResource * debug_utils            = nullptr);

    /// @brief この Block から指定した size の Node を確保する.
    ///
    /// @returns
    ///     確保した Node. 確保できなかった場合は invalid な Node.
    DeviceMemoryNode
    allocateNode(const std::string & label, const std::size_t size, const std::size_t alignment);

    void
    releaseNode(DeviceMemoryBlockNode * const node_ptr) noexcept;

    std::size_t
    getMaxAllocatableNodeSize() const noexcept;

    VkDevice
    getDevice() const noexcept;

    void *
    getMappedPtr() const noexcept;

private:
    void
    _map();

    const std::size_t                                   size_;        // 全体サイズ
    void *                                              mapped_ptr_;  // map されたポインタ
    std::mutex                                          mutex_;       // mutex
    std::unique_ptr<DeviceMemoryBlockNode>              root_node_;   // 一番先頭のルートノード
    std::multimap<std::size_t, DeviceMemoryBlockNode *> free_nodes_;  // 空きノード, size:Node
};

}  // namespace vkwp
/// @endcond

#endif