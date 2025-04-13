#include "device_memory_pool_unit.hpp"

#include "assert.hpp"
#include "vkwp/device_memory_node.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

vkwp::impl::DeviceMemoryPoolUnit::DeviceMemoryPoolUnit(const std::string & label,
                                                       const std::uint32_t memory_type_index,
                                                       const std::size_t   size) noexcept
    : Resource<VkDeviceMemory, VkDevice, void>{ label }
    , memory_type_index_{ memory_type_index }
    , size_{ size }
    , next_id_{ 0 }
    , root_node_{ new Node_{ .size = size } }
    , empty_nodes_{ { size, this->root_node_.get() } }
{
    VKWP_ASSERTION(size > 0);
}

vkwp::impl::DeviceMemoryPoolUnit::~DeviceMemoryPoolUnit() noexcept = default;

/* STATIC */
std::unique_ptr<vkwp::impl::DeviceMemoryPoolUnit>
vkwp::impl::DeviceMemoryPoolUnit::allocate(const std::string & label,
                                           const VkDevice      device,
                                           const std::uint32_t memory_type_index,
                                           const std::size_t   size)
{
    std::unique_ptr<DeviceMemoryPoolUnit> unit{ new DeviceMemoryPoolUnit{ label, memory_type_index, size } };

    const VkMemoryAllocateInfo ai{
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = size,
        .memoryTypeIndex = memory_type_index,
    };
    SPDLOG_DEBUG("DeviceMemory: {} - {}Bytes, {:.03f}MiB",
                 label,
                 memory_requirements.size,
                 static_cast<double>(memory_requirements.size) / 1024.0 / 1024.0);
    VulkanError::check(  //
        vkAllocateMemory(device, &ai, nullptr, unit->reset(device)),
        "Failed to vkAllocateMemory().");
    return unit;
}

vkwp::DeviceMemoryNode
vkwp::impl::DeviceMemoryPoolUnit::allocateNode(const std::string &          label,
                                               const VkMemoryRequirements & memory_requirements)
{
    // bits チェックはここでは行わない.

    // 確保可能な Node を探す
    for (auto iter = this->empty_nodes_.lower_bound(memory_requirements.size); iter != this->empty_nodes_.end(); ++iter)
    {
        // この Node において, 要求された alignment で size 分確保できるかチェックする.
        const auto aligned_offset
            = (((iter->second->offset - 1) / memory_requirements.alignment) + 1) * memory_requirements.alignment;
        if ((iter->second->size - (aligned_offset - iter->second->offset)) <= memory_requirements.size)
        {
            auto * target_node = iter->second;
            auto * prev_node   = target_node->prev;
            this->empty_nodes_.erase(iter);

            // src_node の prev は必ず nullptr か used である.
            VKWP_ASSERTION(prev_node == nullptr || prev_node->used);

            if (target_node->offset != aligned_offset)
            {
                // 確保ノードの前に, フリーノードを作る.
                prev_node->next.reset(new Node_{
                    .prev   = prev_node,
                    .next   = std::move(prev_node->next),  // == target_node
                    .size   = aligned_offset - target_node->offset,
                    .offset = target_node->offset,
                });

                // prev_node をフリーノードにする. (以降, prev_node は作成したフリーノード)
                prev_node = prev_node->next.get();

                // target_node のサイズを縮小し, offset を調整する.
                target_node->size -= prev_node->size;
                target_node->offset = aligned_offset;

                // 作成したフリーノードを empty として登録.
                this->empty_nodes_.emplace(prev_node->size, prev_node);
            }

            if (target_node->size != memory_requirements.size)
            {
                // 確保ノードの後ろに, フリーノードを作る.
                target_node->next.reset(new Node_{
                    .prev   = target_node,
                    .next   = std::move(target_node->next),
                    .size   = target_node->size - memory_requirements.size,
                    .offset = target_node->offset + memory_requirements.size,
                });

                // target_node のサイズを縮小する.
                target_node->size = memory_requirements.size;

                // 作成したフリーノードを empty として登録.
                this->empty_nodes_.emplace(target_node->next->size, target_node->next.get());
            }

            // target_node を used としてマーク.
            target_node->used = true;

            return DeviceMemoryNode{ label, this, target_node, memory_requirements, target_node->offset };
        }
    }

    // 確保できるノードが無かった.
    return DeviceMemoryNode{};
}

void
vkwp::impl::DeviceMemoryPoolUnit::_tryToMergeNextNode(Node_ * const target_node)
{
    // target node が Free かつ
    // next node が Free の場合は merge する.
    if (target_node != nullptr && !target_node->used && target_node->next != nullptr && !target_node->next->used)
    {
        // next node を empty nodes から見つけて, empty nodes から除外する
        for (auto [iter, end] = this->empty_nodes_.equal_range(target_node->next->size); iter != end; ++iter)
        {
            if (iter->second == target_node->next.get())
            {
                this->empty_nodes_.erase(iter);
                break;
            }
        }

        // target node を empty nodes から見つけて, empty nodes から除外する
        for (auto [iter, end] = this->empty_nodes_.equal_range(target_node->size); iter != end; ++iter)
        {
            if (iter->second == target_node)
            {
                this->empty_nodes_.erase(iter);
                break;
            }
        }

        // target_node に next_node 分の容量を足す.
        target_node->size += target_node->next->size;

        // next_node の next を target_node に繋ぎなおし, next_node は消える.
        target_node->next = std::move(target_node->next->next);

        // target_node のサイズを empty nodes に足す.
        this->empty_nodes_.emplace(target_node->size, target_node);
    }
}

void
vkwp::impl::DeviceMemoryPoolUnit::releaseNode(void * const node_ptr) noexcept
{
    VKWP_ASSERTION(node_ptr != nullptr);
    auto * node = reinterpret_cast<Node_ *>(node_ptr);

    // 要件: Free ノードは連続しない.
    // next node が Free であった場合は, next node を reset して, target_node にサイズを統合する.
    // target_node を target_node の prev node に移し, 再度実行.
    // 最高この2回で完了する.
    node->used = false;
    this->_tryToMergeNextNode(node);

    node = node->prev;  // 敢えて. この時点での node->next は, _tryToMergeNextNode() の後, 削除されている可能性がある.
    this->_tryToMergeNextNode(node);
}

std::size_t
vkwp::impl::DeviceMemoryPoolUnit::getMaxAllocatableNodeSize() const noexcept
{
    if (const auto iter = this->empty_nodes_.crbegin(); iter != this->empty_nodes_.crend())
    {
        return iter->first;
    }
    else
    {
        return 0;
    }
}
