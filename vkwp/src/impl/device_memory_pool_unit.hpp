#ifndef VKWP_IMPL_DEVICE_MEMORY_POOL_UNIT_HPP
#define VKWP_IMPL_DEVICE_MEMORY_POOL_UNIT_HPP

#include "vkwp/fwd.hpp"
#include "vkwp/resource.hpp"

#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <unordered_map>

/// @cond
namespace vkwp::impl {

class DeviceMemoryPoolUnit final : public Resource<VkDeviceMemory, VkDevice, void>
{
public:
    DeviceMemoryPoolUnit(const DeviceMemoryPoolUnit &) = delete;
    DeviceMemoryPoolUnit(DeviceMemoryPoolUnit &&)      = delete;

    ~DeviceMemoryPoolUnit() noexcept;

    static std::unique_ptr<DeviceMemoryPoolUnit>
    allocate(const std::string & label,
             const VkDevice      device,
             const std::uint32_t memory_type_index,
             const std::size_t   size);

    DeviceMemoryNode
    allocateNode(const std::string & label, const VkMemoryRequirements & memory_requirements);

    void
    releaseNode(void * const node_ptr) noexcept;

    std::size_t
    getMaxAllocatableNodeSize() const noexcept;

private:
    struct Node_ final
    {
        Node_ *                prev   = nullptr;
        std::unique_ptr<Node_> next   = nullptr;
        std::size_t            size   = 0;
        std::size_t            offset = 0;
        bool                   used   = false;
    };

    DeviceMemoryPoolUnit(const std::string & label,
                         const std::uint32_t memory_type_index,
                         const std::size_t   size) noexcept;

    void
    _tryToMergeNextNode(Node_ * const target_node);

    const std::uint32_t                 memory_type_index_;
    const std::size_t                   size_;
    std::size_t                         next_id_;
    std::unique_ptr<Node_>              root_node_;
    std::multimap<std::size_t, Node_ *> empty_nodes_;  // size:Node
};

}  // namespace vkwp::impl
/// @endcond

#endif