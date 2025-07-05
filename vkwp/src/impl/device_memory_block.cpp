#include "device_memory_block.hpp"

#include "assert.hpp"
#include "device_memory_block_node.hpp"
#include "vkwp/debug_utils_resource.hpp"
#include "vkwp/device_memory_node.hpp"
#include "vkwp/utils.hpp"
#include "vkwp/vulkan_error.hpp"

#include <spdlog/spdlog.h>

vkwp::DeviceMemoryBlock::DeviceMemoryBlock(const std::string & label,
                                           const std::size_t   size,
                                           const std::size_t   mapping_alignment) noexcept
    : Resource<VkDeviceMemory, VkDevice>{ label }
    , size_{ size }
    , mapping_alignment_{ mapping_alignment }
    , root_node_{ std::make_unique<DeviceMemoryBlockNode>(size) }
    , free_nodes_{ { size, this->root_node_.get() } }
{
    VKWP_ASSERTION(size > 0);
}

vkwp::DeviceMemoryBlock::~DeviceMemoryBlock() noexcept = default;

/* STATIC */
std::unique_ptr<vkwp::DeviceMemoryBlock>
vkwp::DeviceMemoryBlock::allocate(const std::string &        label,
                                  const VkDevice             device,
                                  const std::size_t          mapping_alignment,
                                  const std::uint32_t        memory_type_index,
                                  const std::size_t          size,
                                  const DebugUtilsResource * debug_utils)
{
    class DeviceMemoryBlock_ final : public DeviceMemoryBlock
    {
    public:
        DeviceMemoryBlock_(const std::string & l, const std::size_t s, const std::size_t a) noexcept
            : DeviceMemoryBlock{ l, s, a }
        {}
    };

    auto block = std::make_unique<DeviceMemoryBlock_>(label, size, mapping_alignment);

    const VkMemoryAllocateInfo ai{
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = size,
        .memoryTypeIndex = memory_type_index,
    };
    SPDLOG_DEBUG("New memory block is allocated. DeviceMemory: {} - {}Bytes, {:.03f}MiB",
                 label,
                 memory_requirements.size,
                 static_cast<double>(memory_requirements.size) / 1024.0 / 1024.0);
    VulkanError::check(  //
        vkAllocateMemory(device, &ai, nullptr, block->_reset(device)),
        "Failed to vkAllocateMemory().");

    if (debug_utils != nullptr)
    {
        debug_utils->registDebugUtilsObjectName(device,
                                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                                static_cast<VkDeviceMemory>(*block),
                                                block->getLabel());
    }

    return block;
}

vkwp::DeviceMemoryNode
vkwp::DeviceMemoryBlock::allocateNode(const std::string & label, const std::size_t size, const std::size_t alignment)
{
    // 確保可能な Node を探す
    std::lock_guard lock{ this->mutex_ };
    for (auto iter = this->free_nodes_.lower_bound(size); iter != this->free_nodes_.end(); ++iter)
    {
        // この Node において, 要求された alignment で size 分確保できるかチェックする.
        const auto aligned_offset = utils::align(iter->second->getOffset(), alignment);
        if ((iter->second->getSize() - (aligned_offset - iter->second->getOffset())) >= size)
        {
            // 利用する空きノード
            auto * target_node = iter->second;

            // 空きノードから削除する
            this->free_nodes_.erase(iter);

            // まず target_node を used mark する
            target_node->setUsed(true);

            // alignment を取る
            if (auto * const prev_node = target_node->align(this->root_node_, alignment);
                prev_node != nullptr && !prev_node->isUsed())
            {
                // フリーノードとして追加する.
                this->free_nodes_.emplace(prev_node->getSize(), prev_node);
            }

            // 分割する
            if (auto * const next_node = target_node->divide(size);  //
                next_node != nullptr && !next_node->isUsed())
            {
                // フリーノードとして追加する.
                this->free_nodes_.emplace(next_node->getSize(), next_node);
            }

            return DeviceMemoryNode{ label, this->mapping_alignment_, this, target_node };
        }
    }

    // 確保できるノードが無かった.
    return DeviceMemoryNode{};
}

void
vkwp::DeviceMemoryBlock::releaseNode(DeviceMemoryBlockNode * const node) noexcept
{
    // 要件: Free ノードは連続しない.
    // next node が Free であった場合は, next node を reset して, target_node にサイズを統合する.
    // target_node を target_node の prev node に移し, 再度実行.
    // 最大この2回で完了する.

    std::lock_guard lock{ this->mutex_ };

    const auto remove_node_from_free_map_fn = [this](const DeviceMemoryBlockNode & node) {
        for (auto [iter, end] = this->free_nodes_.equal_range(node.getSize()); iter != end; ++iter)
        {
            if (iter->second == &node)
            {
                this->free_nodes_.erase(iter);
                break;
            }
        }
    };

    // まず node に対して.
    if (auto erased_node = node->tryToMergeNextNode(); erased_node)
    {
        // 削除されたノードは free なので, free_nodes_ から回収する.
        remove_node_from_free_map_fn(*erased_node);
    }

    // 次は node の prev に対して
    if (node->getPrev() != nullptr)
    {
        if (auto erased_node = node->getPrev()->tryToMergeNextNode(); erased_node /* == node */)
        {
            VKWP_ASSERTION(node == erased_node.get());

            // 削除されたノードは free なので, free_nodes_ から回収する.
            remove_node_from_free_map_fn(*erased_node);

            // スコープ抜けて以降, node にアクセスしてはいけない.
        }
    }
}

std::size_t
vkwp::DeviceMemoryBlock::getMaxAllocatableNodeSize() const noexcept
{
    if (const auto iter = this->free_nodes_.crbegin(); iter != this->free_nodes_.crend())
    {
        return iter->first;
    }
    else
    {
        return 0;
    }
}

VkDevice
vkwp::DeviceMemoryBlock::getDevice() const noexcept
{
    return std::get<0>(this->parents_);
}
